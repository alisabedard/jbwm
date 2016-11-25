// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef SCREENINFO_H
#define SCREENINFO_H
#include <stdint.h>
#include <X11/Xlib.h>
#include "JBDim.h"
#include "typedef.h"
struct JBWMPixels {
	jbwm_pixel_t fg, bg, fc;
#ifdef JBWM_USE_TITLE_BAR
	jbwm_pixel_t close, resize, shade, stick;
#endif//JBWM_USE_TITLE_BAR
};
struct JBWMScreen {
	GC gc;
	Display * d;
	union {
		jbwm_window_t root, r;
	};
#ifdef JBWM_USE_EWMH
	jbwm_window_t supporting;
#endif//JBWM_USE_EWMH
	struct JBWMPixels pixels;
	struct JBDim size;
	uint8_t screen, vdesk;
} __attribute__((packed));
#endif /* not SCREENINFO_H */
