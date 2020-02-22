// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020,Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015,Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "ewmh.h"
#include "atom.h"
#include "config.h"
#include "JBWMClient.h"
#include "log.h"
#include "macros.h"
#include "PropertyData.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"
#include <X11/Xatom.h>
static inline void set_property(struct PropertyData * restrict p) {
    jbwm_set_property(p->display,p->target,p->property,
        p->type,p->data,p->size);
}
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
    jbwm_set_property(d,head->screen->xlib->root,a, XA_WINDOW, window_list,n);
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
    jbwm_set_property(d, root,a,XA_WINDOW,window_list,n);
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

void jbwm_ewmh_set_allowed_actions(Display * d,
    Window const w)
{
    Atom a[]={
        jbwm_atoms[JBWM_NET_WM_ACTION_MOVE],
        jbwm_atoms[JBWM_NET_WM_ACTION_RESIZE],
        jbwm_atoms[JBWM_NET_WM_ACTION_CLOSE],
        jbwm_atoms[JBWM_NET_WM_ACTION_SHADE],
        jbwm_atoms[JBWM_NET_WM_ACTION_FULLSCREEN],
        jbwm_atoms[JBWM_NET_WM_ACTION_CHANGE_DESKTOP],
        jbwm_atoms[JBWM_NET_WM_ACTION_ABOVE],
        jbwm_atoms[JBWM_NET_WM_ACTION_BELOW],
        jbwm_atoms[JBWM_NET_WM_ACTION_MAXIMIZE_HORZ],
        jbwm_atoms[JBWM_NET_WM_ACTION_MAXIMIZE_VERT],
    };
    jbwm_set_property(d,w,a[0],XA_ATOM,&a,sizeof(a) / sizeof(Atom));
}
static void set_desktop_geometry(struct PropertyData * restrict p,
    struct JBWMScreen *s)
{
    p->property=jbwm_atoms[JBWM_NET_DESKTOP_GEOMETRY];
    p->data=(int32_t[]){s->xlib->width,s->xlib->height};
    set_property(p);
}
static void set_desktop_viewport(struct PropertyData * restrict p)
{
    int32_t viewport_data[]={0,0};
    p->property=jbwm_atoms[JBWM_NET_DESKTOP_VIEWPORT];
    p->data=viewport_data;
    set_property(p);
}
static void set_number_of_desktops(struct PropertyData * restrict p)
{
    p->property=jbwm_atoms[JBWM_NET_NUMBER_OF_DESKTOPS];
    p->data=&(int32_t){JBWM_NUMBER_OF_DESKTOPS};
    set_property(p);
}
static void set_current_desktop(struct PropertyData * restrict p,
    void * restrict data)
{
    p->property=jbwm_atoms[JBWM_NET_CURRENT_DESKTOP];
    p->data=data;
    set_property(p);
}
static void set_virtual_roots(struct PropertyData * restrict p)
{
    Window r;
    p->property=jbwm_atoms[JBWM_NET_VIRTUAL_ROOTS];
    p->type=XA_WINDOW;
    r=p->target;
    p->data=&r;
    set_property(p);
}
static void init_desktops(Display * d,struct JBWMScreen * s)
{
    struct PropertyData p={d,NULL,RootWindowOfScreen(s->xlib),
        0,2,XA_CARDINAL};
    set_desktop_geometry(&p,s);
    set_desktop_viewport(&p);
    p.size=1; // single dimension data follows
    set_number_of_desktops(&p);
    set_current_desktop(&p,&s->vdesk);
    set_virtual_roots(&p);
}
static inline void set_name(Display * d,Window const w)
{
    jbwm_set_property(d,w,jbwm_atoms[JBWM_NET_WM_NAME],XA_STRING,
        JBWM_NAME,sizeof(JBWM_NAME));
}
static void set_supporting(Display * d,Window const w,
    Window * s)
{
    Atom a;
    a=jbwm_atoms[JBWM_NET_SUPPORTING_WM_CHECK];
    jbwm_set_property(d,w,a,XA_WINDOW,s,1);
}
static Window init_supporting(Display * d,Window const r)
{
    Window w;
    w=XCreateSimpleWindow(d,r,0,0,1,1,0,0,0);
    set_supporting(d,r,&w);
    set_supporting(d,w,&w);
    jbwm_set_property(d,w,jbwm_atoms[JBWM_NET_WM_PID],XA_CARDINAL,
        &(pid_t){getpid()},1);
    set_name(d,w);
    return w;
}
static void set_ewmh_client_list(Display *d,Window *r){
    Atom a;
    /* Set this to the root window until we have some clients.
     * Declared r static so we don't lose scope.  */
    a=jbwm_atoms[JBWM_NET_CLIENT_LIST];
    jbwm_set_property(d,*r,a,XA_WINDOW,r,1);
}
static void set_ewmh_supported(Display *d,Window const r){
    XInternAtoms(d,jbwm_atom_names,JBWM_ATOM_COUNT,false,jbwm_atoms);
    jbwm_set_property(d,r,jbwm_atoms[JBWM_NET_SUPPORTED],
        XA_ATOM,(unsigned char *)&jbwm_atoms,JBWM_ATOM_COUNT);
}
void jbwm_ewmh_init_screen(Display * d,struct JBWMScreen * s)
{
    static Window r;
    r=RootWindowOfScreen(s->xlib);
    set_ewmh_supported(d,r);
    set_name(d,r);
    set_ewmh_client_list(d,&r);
    init_desktops(d,s);
    s->supporting=init_supporting(d,r);
}
// Required by wm-spec:
void jbwm_set_frame_extents(struct JBWMClient * restrict c)
{
    static uint32_t f[4];
    Atom a;
    JBWM_LOG("jbwm_set_frame_extents()");
    // Fields: left,right,top,bottom
    f[0]=f[1]=f[2]=f[3]=c->opt.border;
    if (!c->opt.no_title_bar)
        f[2] += c->screen->font_height;
    a=jbwm_atoms[JBWM_NET_FRAME_EXTENTS];
    jbwm_set_property(c->screen->display,c->parent,a,XA_CARDINAL,f,4);
}
