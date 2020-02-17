// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "vdesk.h"
#include <X11/Xatom.h> // keep
#include "JBWMClient.h"
#include "JBWMScreen.h"
#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "util.h"
static void check_visibility(struct JBWMClient * restrict c,
    const uint8_t v)
{
    if (c) {
        if (c->vdesk == v || c->opt.sticky) {
            // allow moving windows by sticking
            c->vdesk = v;
            jbwm_restore_client(c);
        } else
            jbwm_hide_client(c);
        check_visibility(c->next, v);
    }
}
uint8_t jbwm_set_vdesk(struct JBWMScreen *s,
    struct JBWMClient *head, uint32_t v)
{
    Display *d;
    check_visibility(head, v);
    s->vdesk = v;
    d=s->display;
    // The data (v) must be a 32 bit type.
    jbwm_set_property(d, s->xlib->root,
        XInternAtom(d,"_NET_CURRENT_DESKTOP",false),
        XA_CARDINAL, &v, 1);
    return v;
}
