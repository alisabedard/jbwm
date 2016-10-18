// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SHAPE_H
#define JBWM_SHAPE_H
#ifdef USE_SHAPE
#include "JBWMClient.h"
void setup_shaped(struct JBWMClient * c)
	__attribute__((nonnull));
void set_shape(struct JBWMClient * c)
	__attribute__((nonnull));
#else//!USE_SHAPE
#define setup_shaped(c)
#define set_shape(c)
#endif//USE_SHAPE
#endif//!JBWM_SHAPE_H
