// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_EWMH_H
#define JBWM_EWMH_H
#include <X11/Xlib.h>
#include <stdint.h>
struct JBWMClient;
struct JBWMScreen;
void jbwm_ewmh_update_client_list(Display * d);
void jbwm_ewmh_set_allowed_actions(Display * d,
    const Window w) __attribute__((nonnull));
void jbwm_ewmh_init_screen(Display * d, struct JBWMScreen * s)
__attribute__((nonnull));
void jbwm_set_frame_extents(struct JBWMClient * restrict c);
#endif//!JBWM_EWMH_H
