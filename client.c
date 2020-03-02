/*  jbwm - Minimalist Window Manager for X */
/*  Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com> */
/*  Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk> */
/*  See README for license and other details. */
#include "client.h"
#include <stdlib.h>
#include "ewmh.h"
#include "ewmh_state.h"
#include "atom.h"
#include "select.h"
#include "title_bar.h"
#include "util.h"
#include "vdesk.h"
#include "wm_state.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
/*  Relink client linked list to exclude c */
void jbwm_relink_client_list(struct JBWMClient * c,
    struct JBWMClient ** head_client)
{
    struct JBWMClient *i, *prev;
    for (i=*head_client, prev=NULL; i; prev=i, i=i->next) {
        if (i==c) {
           if (i==*head_client) { // prev == NULL in this case
               *head_client=i->next;
           } else {
               prev->next=i->next;
           } 
           break;
        }
    }
}
void jbwm_set_client_vdesk(struct JBWMClient * restrict c, uint8_t desktop)
{
    Display *d;
    if(c){
        d=c->screen->display;
        c->vdesk=desktop;
        /*  Save in an atomic property, useful for restart and deskbars. */
        XChangeProperty(d, c->window, jbwm_atoms[JBWM_NET_WM_DESKTOP],
            XA_CARDINAL, 8, PropModeReplace, (unsigned char *)&desktop, 1);
        if(c->screen->vdesk!=c->vdesk)
            jbwm_hide_client(c);
    }
}
/* Return the client that has specified window as either window or parent.
 *  This is the third most called function.  Show restraint in adding any
 *  future tests.  */
struct JBWMClient * jbwm_find_client(
    struct JBWMClient * restrict head, const Window w)
{
    return !head || head->parent == w || head->window == w
        || head->tb.win == w ? head : jbwm_find_client(head->next, w);
}

void jbwm_toggle_sticky(struct JBWMClient * restrict c,
    struct JBWMClient ** current_client)
{
    if(c){
        c->opt.sticky ^= true; /*  toggle */
        jbwm_select_client(c,current_client);
        jbwm_update_title_bar(c);
        {
            Display *d;
            d=c->screen->display;
            (c->opt.sticky ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
            (d, c->window,
                jbwm_atoms[JBWM_NET_WM_STATE_STICKY]);
        }
    }
}
/*  Free client and destroy its windows and properties. */
void jbwm_client_free(struct JBWMClient * c, struct JBWMClient ** head_client)
{
    Display *d;
    const Window w = c->window, parent = c->parent;
    const union JBWMRectangle * restrict p = &c->size;
    d = c->screen->display;
    /*  Per ICCCM + wm-spec */
    XDeleteProperty(d, w, jbwm_atoms[JBWM_NET_WM_STATE]);
    XDeleteProperty(d, w, jbwm_atoms[JBWM_NET_WM_DESKTOP]);
    XReparentWindow(d, w, c->screen->xlib->root, p->x, p->y);
    XRemoveFromSaveSet(d, w);
    if(parent)
        XDestroyWindow(d, parent);
    jbwm_relink_client_list(c, head_client);
    free(c);
}
void jbwm_hide_client(const struct JBWMClient * restrict c)
{
    XUnmapWindow(c->screen->display, c->parent);
    jbwm_set_wm_state(c, IconicState);
}
void jbwm_restore_client(const struct JBWMClient * restrict c)
{
    XMapWindow(c->screen->display, c->parent);
    jbwm_set_wm_state(c, NormalState);
}
