// Copyright 2020, Jeffrey E. Bedard
#include "wm_state.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "ewmh.h"
#include "ewmh_state.h"
#include "log.h"
#include "util.h"
// Returns 0 on failure.
static Status xmsg(Display * restrict d, const Window w,
    const Atom a, const long x)
{
    JBWM_LOG("xmsg");
    XClientMessageEvent e = { .type = ClientMessage, .window = w,
        .message_type = a, .format = 32 };
    // Split initialization to avoid old gcc warnings.
    { // * l scope
        long * restrict l = e.data.l;
        l[0] = x;
        l[1] = CurrentTime;
    }
    return XSendEvent(d, w, false, NoEventMask, (void*)&e);
}
static Atom get_atom(Display * restrict d,
    Atom * restrict a, const char * restrict name)
{
    return *a ? *a : (*a = XInternAtom(d, name, false));
}
#define ATOM_GETTER(func, atom) Atom func(Display * d) \
{\
    static Atom a;\
    return get_atom(d, &a, atom);\
}
static ATOM_GETTER(get_wm_protocols, "WM_PROTOCOLS")
static ATOM_GETTER(get_wm_delete_window, "WM_DELETE_WINDOW")
ATOM_GETTER(jbwm_get_wm_state, "WM_STATE")
void jbwm_set_wm_state(const struct JBWMClient * restrict c,
    const int8_t state)
{
    const Window w = c->window;
    Display * d = c->screen->display;
    jbwm_set_property(d, w, jbwm_get_wm_state(d), XA_CARDINAL,
        &(uint32_t){state}, 1);
    (state == IconicState ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
    (d, w, XInternAtom(d,"_NET_WM_STATE_HIDDEN",false));
}
static bool find_delete_proto(const int i, const Atom * restrict p,
    Display * d)
{
    return (i < 0) ? false : p[i] == get_wm_delete_window(d) ? true
    : find_delete_proto(i - 1, p, d);
}
static bool has_delete_proto(const Window w, Display * d)
{
    bool found = false;
    { // * p, i scope
        Atom * p;
        int i;
        if (XGetWMProtocols(d, w, &p, &i)) {
            found = find_delete_proto(i, p, d);
            XFree(p);
        }
    }
    return found;
}
void jbwm_send_wm_delete(struct JBWMClient * restrict c)
{
    const Window w = c->window;
    Display * d = c->screen->display;
    if (c->opt.remove)
        // this allows a second click to force a kill
        XKillClient(d, w);
    else {
        c->opt.remove = true;
        if (has_delete_proto(w, d))
            xmsg(d, w, get_wm_protocols(d),
                get_wm_delete_window(d));
        else // client does not support protocol, force kill:
            XKillClient(d, w);
    }
    // Unmap early in case window does not disappear immediately.
    XUnmapWindow(d, c->parent);
    XFlush(d); // Do everything now.
}
