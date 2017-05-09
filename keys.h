// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_KEYS_H
#define JBWM_KEYS_H
#include <X11/Xlib.h>
struct JBWMScreen;
void jbwm_grab_screen_keys(Display * d, struct JBWMScreen * s)
	__attribute__((nonnull));
#endif//!JBWM_KEYS_H
