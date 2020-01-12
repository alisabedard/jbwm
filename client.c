// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "client.h"
#include <X11/Xutil.h>
#include <stdlib.h>
#include "JBWMAtomIndex.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "select.h"
#include "title_bar.h"
#include "vdesk.h"
#include "wm_state.h"
#define EWMH_ATOM(suffix) jbwm_ewmh_get_atom(JBWM_EWMH_##suffix)
static struct JBWMClient * current, * head;
struct JBWMClient * jbwm_get_current_client(void)
{
    return current;
}
void jbwm_set_current_client(struct JBWMClient * restrict c)
{
    current = c;
}
struct JBWMClient * jbwm_get_head_client(void)
{
    return head;
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
// Relink c's linked list to exclude c
void jbwm_relink_client_list(struct JBWMClient * restrict c)
{
    if (current == c) // Remove selection target
        current = NULL;
    if (head == c) {
        head = c->next;
        return; // removed first client
    }
    relink_r(c, head);
}
void jbwm_set_client_vdesk(struct JBWMClient * restrict c,
    const uint8_t desktop)
{
    const uint8_t p = c->vdesk; // save previous
    c->vdesk = desktop;
    // use jbwm_set_vdesk to validate desktop:
    c->vdesk = jbwm_set_vdesk(c->display, c->screen, desktop);
    jbwm_set_vdesk(c->display, c->screen, p);
}
/*  This is the third most called function.  Show restraint in adding any
 *  future tests.  */
static inline bool matches(struct JBWMClient * restrict i, const Window w)
{
    return i->parent == w || i->window == w || i->tb.win == w;
}
__attribute__((pure))
static struct JBWMClient * search_client_for_window(
    struct JBWMClient * restrict c, const Window w)
{
    return !c || matches(c, w) ? c : search_client_for_window(c->next, w);
}
// Return the client that has specified window as either window or parent
__attribute__((pure))
struct JBWMClient * jbwm_get_client(const Window w)
{
    return search_client_for_window(head, w);
}
void jbwm_toggle_sticky(struct JBWMClient * restrict c)
{
    c->opt.sticky ^= true; // toggle
    jbwm_select_client(c);
    jbwm_update_title_bar(c);
    (c->opt.sticky ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
        (c->display, c->window, EWMH_ATOM(WM_STATE_STICKY));
}
// Free client and destroy its windows and properties.
void jbwm_client_free(struct JBWMClient * restrict c)
{
    const Window w = c->window, parent = c->parent;
    const struct JBWMRectangle * restrict p = &c->size;
    Display * d = c->display;
    // Per ICCCM + wm-spec
    XDeleteProperty(d, w, EWMH_ATOM(WM_STATE));
    XDeleteProperty(d, w, EWMH_ATOM(WM_DESKTOP));
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
    mapping(c->display, c->parent);
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
