// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <stdint.h>
#include <X11/Xlib.h>
#include "geometry.h"

typedef struct ScreenInfo {
	Window root : 32;
#ifdef EWMH
	Window supporting : 32;
#endif//EWMH
	GC gc;
	Dim size;
	uint8_t screen : 4, vdesk : 4;
	struct {
		uint32_t fg, bg, fc;
	} pixels;
} ScreenInfo;

#endif /* not SCREENINFO_H */
