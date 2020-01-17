// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "events.h"
#include <X11/Xatom.h>
#include "button_event.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_client.h"
#include "JBWMScreen.h"
#include "key_event.h"
#include "log.h"
#include "move_resize.h"
#include "new.h"
#include "select.h"
#include "title_bar.h"
#include "util.h"
#include "wm_state.h"
// Set log level for events
#define JBWM_LOG_EVENTS 5
static struct JBWMScreen * get_screen(struct JBWMScreen *s,
    const Window root, const int i) {
    return RootWindowOfScreen(s[i].xlib) == root ? s
        // Check that the next iteration is not outside array bounds, using
        // suppporting field as validation
        : s[i + 1].supporting ? get_screen(s,root, i + 1) : s;
}
static void cleanup(Display * d, struct JBWMClient * i)
{
    JBWM_LOG("cleanup");
    if (!i)
        return;
    struct JBWMClient * next = i->next; // save
    if (i->opt.remove)
        jbwm_client_free(i);
    cleanup(d, next);
}
static void jbwm_handle_PropertyNotify(XEvent * ev, struct JBWMClient * c)
{
    if (c) {
        XPropertyEvent * restrict e = &ev->xproperty;
        if (e->state != PropertyNewValue)
            return;
        if (e->atom == XA_WM_NAME)
            jbwm_update_title_bar(c);
        else {
            Display * d = e->display;
#if JBWM_LOG_EVENTS > 3
            jbwm_print_atom(d, e->atom, __FILE__, __LINE__);
#endif//JBWM_LOG_EVENTS > 3
            if (e->atom == jbwm_get_wm_state(d))
                jbwm_move_resize(c);
        }
    }
}
static void jbwm_handle_MapRequest(XEvent * ev, struct JBWMClient * c,
    struct JBWMScreen * s) {
    if (!c) {
        XMapRequestEvent * restrict e = &ev->xmaprequest;
        /* This check fixes a race condition in old libreoffice and
         * certain Motif dialogs where an attempt is made to
         * request mapping twice: */
        static unsigned long serial;
        if (e->serial == serial)
            return;
        serial = e->serial;
        JBWM_LOG("jbwm_handle_MapRequest(), send_event:%d",
            e->send_event);
        jbwm_new_client(e->display, get_screen(s, e->parent, 0),
            e->window);
    }
}
static inline bool mark_removal(struct JBWMClient * restrict c)
{
    JBWM_LOG("mark_removal(): ignore_unmap is %d", c->ignore_unmap);
    return c->opt.remove = (c->ignore_unmap--<1);
}
static void jbwm_handle_ColormapNotify(XEvent * ev, struct JBWMClient * c)
{
    XColormapEvent * restrict e = &ev->xcolormap;
    if (c && e->new)
        XInstallColormap(e->display, c->cmap = e->colormap);
}
static void jbwm_handle_ConfigureNotify(XEvent * ev, struct JBWMClient * c)
{
    if (c && !ev->xconfigure.override_redirect)
        jbwm_move_resize(c);
}
static void jbwm_handle_ConfigureRequest(XEvent * ev, struct JBWMClient * c)
{
    XConfigureRequestEvent * restrict e = &ev->xconfigurerequest;
    JBWM_LOG("handle_configure_request():"
        "x: %d, y: %d, w: %d, h: %d, b: %d",
        e->x, e->y, e->width, e->height, e->border_width);
    XConfigureWindow(e->display, e->window, e->value_mask,
        &(XWindowChanges){ .x = e->x, .y = e->y,
            .width = e->width, .height = e->height,
            .border_width = e->border_width,
            .sibling = e->above, .stack_mode = e->detail});
    XSync(e->display, false);
    if (c)
        jbwm_move_resize(c);
}
static void jbwm_handle_EnterNotify(XEvent * ev, struct JBWMClient * c)
{
    if (c && ev->xcrossing.window == c->parent)
        jbwm_select_client(c);
}
static void jbwm_handle_Expose(XEvent * ev, struct JBWMClient * c)
{
    if (c && !ev->xexpose.count)
        jbwm_update_title_bar(c);
}
void jbwm_events_loop(struct JBWMScreen * s)
{
    static bool need_cleanup;
    Display *d;
    d=s->display;
    for (;;) {
        XEvent ev;
        XNextEvent(d, &ev);
        struct JBWMClient * c = jbwm_get_client(ev.xany.window);
        if(c)
            s=c->screen; // refer to the client's local screen
        switch (ev.type) {
#define ECASE(name) case name: JBWM_LOG("\tXEVENT: %s", #name);\
            jbwm_handle_##name(&ev, c); break;
        case ButtonRelease:
        case KeyRelease:
        case MapNotify:
        case MappingNotify:
        case MotionNotify:
        case ReparentNotify:
            // ignore
            break;
            ECASE(ConfigureNotify);
            ECASE(ConfigureRequest);
        case KeyPress:
            jbwm_handle_key_event(c?c->screen:s,&ev.xkey);
            break;
        case ButtonPress:
            if(c)
                jbwm_handle_button_event(&ev.xbutton, c);
            break;
            ECASE(EnterNotify);
            ECASE(Expose);
        case CreateNotify:
            if (ev.xcreatewindow.override_redirect) // internal
                jbwm_ewmh_update_client_list(d);
            break;
        case DestroyNotify:
            if (!c) // only bother if event was not on a client
                jbwm_ewmh_update_client_list(d);
            break;
        case UnmapNotify:
            if (c)
                need_cleanup=mark_removal(c);
            break;
        case MapRequest:
            jbwm_handle_MapRequest(&ev,c,s);
            break;
            ECASE(PropertyNotify);
            ECASE(ColormapNotify);
        case ClientMessage:
            jbwm_ewmh_handle_client_message(&ev.xclient, c, s);
            break;
#ifdef DEBUG
        default:
            JBWM_LOG("Unhandled event %d", ev.type);
#endif//DEBUG
        }
        if (need_cleanup) {
            cleanup(d, jbwm_get_head_client());
            need_cleanup = false;
        }
    }
}

