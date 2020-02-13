// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "select.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "JBWMClient.h"
#include "util.h"
#define WM_STATE(a) EWMH_ATOM(WM_STATE_##a)
static inline jbwm_pixel_t get_bg(struct JBWMClient * c)
{
    return c->screen->pixels.bg;
}
static void set_state_not_focused(struct JBWMClient * c)
{
    Display *d=c->screen->display;
    XSetWindowBorder(d, c->parent, get_bg(c));
    jbwm_ewmh_remove_state(d, c->window, XInternAtom(d,
            "_NET_WM_STATE_FOCUSED",false));
}
static void unselect_current(struct JBWMClient * c)
{
    struct JBWMClient * current = jbwm_get_current_client();
    if (current && current != c)
        set_state_not_focused(current);
}
static void set_border(struct JBWMClient * c)
{
    struct JBWMPixels * restrict p = &c->screen->pixels;
    XSetWindowBorder(c->screen->display, c->parent,
        c->opt.sticky ? p->fc : p->fg);
}
static void set_focused(struct JBWMClient * c)
{
    Display * d = c->screen->display;
    XInstallColormap(d, c->cmap);
    const Window w = c->window;
    XSetInputFocus(d, w, RevertToPointerRoot, CurrentTime);
    jbwm_ewmh_add_state(d, w, XInternAtom(d,"_NET_WM_STATE_FOCUSED",false));
}
static void set_active_window(struct JBWMClient * c)
{
    /* Store the window id as a static variable here in case
     * client c is freed before the X server handles the event.
     * If the property is read after the client is freed, it will
     * cause a segmentation fault.  */
    static Window w;
    Display *d;
    w = c->window;
    d=c->screen->display;
    jbwm_set_property(d, c->screen->xlib->root,
        XInternAtom(d,"_NET_ACTIVE_WINDOW",false), XA_WINDOW, &w, 1);
    jbwm_set_current_client(c);
}
void jbwm_select_client(struct JBWMClient * c)
{
    if (!c)
        return;
    unselect_current(c);
    set_border(c);
    set_focused(c);
    set_active_window(c);
}

