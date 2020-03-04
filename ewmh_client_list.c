#include "ewmh_client_list.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#include "JBWMClient.h"
#include "atom.h"
#include "config.h"
#include "log.h"
#include "macros.h"
// returns number of elements in window list
static int get_client_list_r(Window ** list,Display * d,
    struct JBWMClient * i,int const count) {
    int rval;
    if (i) {
        int const new_count =count + 1;
        *list=realloc(*list,new_count  * sizeof(Window));
        (*list)[count]= i->window;
        rval=get_client_list_r(list,d,i->next,new_count);
    } else
        rval=count;
    return rval;
}
#ifdef JBWM_EWMH_DEBUG_WINDOW_LIST
static void debug_window_list(int n,Window * window_list)
{
    if (n > 0) {
        JBWM_LOG("window_list[%d] is 0x%x",
            n,(int)window_list[n]);
        debug_window_list(n - 1,window_list);
    }
}
#else//!JBWM_EWMH_DEBUG_WINDOW_LIST
#define debug_window_list(n,w)
#endif//DEBUG
static Window * get_mixed_client_list(struct JBWMClient *head)
{
    static Window * window_list;
    Display *d;
    Atom a;
    int n;
    if (window_list) {
        free(window_list);
        window_list=NULL;
    }
    d=head->screen->display;
    n=get_client_list_r(&window_list,d,head,0);
    a=jbwm_atoms[JBWM_NET_CLIENT_LIST];
    XChangeProperty(d, head->screen->xlib->root, a, XA_WINDOW, 32,
        PropModeReplace, (unsigned char *)window_list, n);
    debug_window_list(n,window_list);
    return window_list;
}
static inline bool query_tree(Display * d,Window ** children_return,
    unsigned int * restrict nchildren_return)
{
    Window parent,root;
    bool r;
    if(d&&children_return&&nchildren_return)
        r=XQueryTree(d,DefaultRootWindow(d),&parent,&root,children_return,
            nchildren_return);
    else
        r=false;
    return r;
}
static unsigned int get_window_list(Display * d,uint8_t const max_clients,
    Window * window_list)
{
    Window * wl;
    unsigned int n;
    if (query_tree(d,&wl,&n)) {
        n=JB_MIN(n,max_clients); // limit to MAX_CLIENTS
        memcpy(window_list,wl,n * sizeof(Window));
        debug_window_list(n,wl);
        XFree(wl);
    }
    return n;
}
static Window * get_ordered_client_list(Display * d, Window const root)
{
    enum {MAX_CLIENTS=64};
    static Window window_list[MAX_CLIENTS];
    Atom a;
    // get ordered list of all windows on default screen:
    unsigned int const n=get_window_list(d,MAX_CLIENTS,window_list);
    JBWM_LOG("get_ordered_client_list() n: %d",(int)n);
    a=jbwm_atoms[JBWM_NET_CLIENT_LIST_STACKING];
    XChangeProperty(d, root,a,XA_WINDOW, 32, PropModeReplace,
        (unsigned char *)window_list,n);
    return window_list;
}
void jbwm_ewmh_update_client_list(struct JBWMClient *head)
{
    if(head){ // there may be no clients
        get_mixed_client_list(head);
        get_ordered_client_list(head->screen->display,
            head->screen->xlib->root);
    }
}


