// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_JBWMSCREEN_H
#define JBWM_JBWMSCREEN_H
#include <stdint.h>
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include "JBWMPixels.h"
struct JBWMScreen {
	Screen * xlib;
	XftDraw * xft;
	XftColor font_color;
#ifdef JBWM_USE_EWMH
	Window supporting;
#endif//JBWM_USE_EWMH
	struct JBWMPixels pixels;
	uint8_t id, vdesk;
	const int8_t __pad[2];
};
#endif//!JBWM_JBWMSCREEN_H
