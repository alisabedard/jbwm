// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <stdint.h>
#include <X11/Xlib.h>

typedef uint32_t jbwm_window_t;
typedef uint32_t jbwm_pixel_t;
typedef uint16_t jbwm_dim_t;
typedef int16_t jbwm_pos_t;

enum {
	JBWM_SIZE_WIDTH,
	JBWM_SIZE_HEIGHT,
};

typedef struct ScreenInfo {
	GC gc;
	union {
		jbwm_window_t root, r;
	};
#ifdef EWMH
	jbwm_window_t supporting;
#endif//EWMH
	struct {
		jbwm_pixel_t fg, bg, fc;
	} pixels;
	jbwm_dim_t size[2];
	uint8_t screen, vdesk;
} ScreenInfo;

#endif /* not SCREENINFO_H */
