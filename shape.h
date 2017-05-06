// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SHAPE_H
#define JBWM_SHAPE_H
#ifdef JBWM_USE_SHAPE
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_new_shaped_client(Display * d, struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_set_shape(Display * d, struct JBWMClient * restrict c)
	__attribute__((nonnull));
#else//!JBWM_USE_SHAPE
#define jbwm_new_shaped_client(d, c)
#define jbwm_set_shape(d, c)
#endif//JBWM_USE_SHAPE
#endif//!JBWM_SHAPE_H
