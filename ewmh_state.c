// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#undef DEBUG
#include "ewmh_state.h"
#include <assert.h>
#include "JBWMAtomIndex.h"
#include "client.h"
#include "drag.h"
#include "ewmh.h"
#include "jbwm.h"
#include "log.h"
#include "max.h"
#include "select.h"
#include "util.h"
#include "wm_state.h"
#include <X11/Xatom.h>
// Remove specified atom from WM_STATE
void jbwm_ewmh_remove_state(Display * d,
    const Window w, const Atom state)
{
    uint16_t n;
    const Atom ws = XInternAtom(d,"_NET_WM_STATE",false);
    Atom *a = jbwm_get_property(d, w, ws, &n);
    if(a){ // found property
        uint16_t i;
        for(i=0;i<n;++i)
            if(a[i]==state)
                a[i]=0;
        jbwm_set_property(d, w, ws, XA_ATOM, a, n);
        XFree(a);
    }
}
static bool find_state(const Atom state, const int n, Atom * restrict a)
{
    return n ? a[n] == state ? true : find_state(state, n - 1, a) : false;
}
bool jbwm_ewmh_get_state(Display * d,
    const Window w, const Atom state)
{
    uint16_t n;
    Atom *a = jbwm_get_property(d, w,
        XInternAtom(d,"_NET_WM_STATE",false), &n);
    bool found = false;
    if (a) {
        found = find_state(state, n, a);
        XFree(a);
    }
    return found;
}
void jbwm_ewmh_add_state(Display * d, const Window w,
    Atom state)
{
    XChangeProperty(d, w, XInternAtom(d,"_NET_WM_STATE",false),
        XA_ATOM, 32, PropModePrepend,
        (unsigned char *)&state, 1);
}

