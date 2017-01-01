// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_JBWMSCREEN_H
#define JBWM_JBWMSCREEN_H
#include <stdint.h>
#include <X11/Xlib.h>
#include "JBWMSize.h"
#include "pixel_t.h"
struct JBWMPixels {
	jbwm_pixel_t fg, bg, fc;
#ifdef JBWM_USE_TITLE_BAR
	jbwm_pixel_t close, resize, shade, stick;
#endif//JBWM_USE_TITLE_BAR
};
struct JBWMScreen {
	GC gc;
	Window root;
#ifdef JBWM_USE_EWMH
	Window supporting;
#endif//JBWM_USE_EWMH
	struct JBWMPixels pixels;
	struct JBWMSize size;
	uint8_t screen, vdesk;
} __attribute__((packed));
#endif//!JBWM_JBWMSCREEN_H
