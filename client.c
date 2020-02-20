/*  jbwm - Minimalist Window Manager for X */
/*  Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com> */
/*  Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk> */
/*  See README for license and other details. */
#include "client.h"
#include <stdlib.h>
#include "ewmh.h"
#include "ewmh_state.h"
#include "JBWMAtomName.h"
#include "select.h"
#include "title_bar.h"
#include "util.h"
#include "vdesk.h"
#include "wm_state.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
static struct JBWMClient * current, * head;
struct JBWMClient * jbwm_get_current_client(void)
{
    return current;
}
void jbwm_set_current_client(struct JBWMClient * c)
{
    current = c;
}
struct JBWMClient **jbwm_get_head_client(void){
    return &head;
}
void jbwm_prepend_client(struct JBWMClient * restrict c)
{
    c->next = head;
    head = c;
}
/* Note:  As *c and *i may alias each other, use of 'restrict'
   in relink_r would be invalid. */
static void relink_r(const struct JBWMClient * c, struct JBWMClient * i)
{
    if (i && i->next)
        relink_r(c, i->next != c ? i->next : (i->next = c->next));
}
/*  Relink c's linked list to exclude c */
void jbwm_relink_client_list(struct JBWMClient * restrict c)
{
    if (current == c) /*  Remove selection target */
        current = NULL;
    if (head == c) {
        head = c->next;
        return; /*  removed first client */
    }
    relink_r(c, head);
}
void jbwm_set_client_vdesk(struct JBWMClient * restrict c,
    const uint8_t desktop)
{
    Display *d;
    Atom a;
    if(c){
        d=c->screen->display;
        a=jbwm_atoms[JBWM_NET_WM_DESKTOP];
        c->vdesk=desktop;
        /*  Save in an atomic property, useful for restart and deskbars. */
        jbwm_set_property(d, c->window, a, XA_CARDINAL,
            &(int32_t){desktop}, 1);
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

void jbwm_toggle_sticky(struct JBWMClient * restrict c)
{
    if(c){
        c->opt.sticky ^= true; /*  toggle */
        jbwm_select_client(c);
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
void jbwm_client_free(struct JBWMClient * restrict c)
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
    jbwm_relink_client_list(c);
    free(c);
}
static void set_showing(const struct JBWMClient * restrict c,
    int (* mapping)(Display *, Window),
    const int8_t wm_state)
{
    mapping(c->screen->display, c->parent);
    jbwm_set_wm_state(c, wm_state);
}
void jbwm_hide_client(const struct JBWMClient * restrict c)
{
    set_showing(c, XUnmapWindow, IconicState);
}
void jbwm_restore_client(const struct JBWMClient * restrict c)
{
    set_showing(c, XMapWindow, NormalState);
}
