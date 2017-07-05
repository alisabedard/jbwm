// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_EWMH_H
#define JBWM_EWMH_H
#ifdef JBWM_USE_EWMH
#include <X11/Xlib.h>
#include <stdint.h>
struct JBWMClient;
struct JBWMScreen;
Atom jbwm_ewmh_get_atom(const uint8_t index);
void jbwm_ewmh_update_client_list(Display * d);
void jbwm_ewmh_set_allowed_actions(Display * d,
	const Window w) __attribute__((nonnull));
void jbwm_ewmh_init_screen(Display * d, struct JBWMScreen * s)
	__attribute__((nonnull));
void jbwm_set_frame_extents(struct JBWMClient * restrict c);
#else//!JBWM_USE_EWMH
#define jbwm_ewmh_get_atom(i)
#define jbwm_ewmh_set_allowed_actions(d, w)
#define jbwm_ewmh_init_screen(d, s)
#define jbwm_ewmh_init(d)
#define jbwm_ewmh_update_client_list(d)
#define jbwm_set_frame_extents(c)
#endif//JBWM_USE_EWMH
#endif//!JBWM_EWMH_H
