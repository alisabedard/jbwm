// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_JBWMSCREEN_H
#define JBWM_JBWMSCREEN_H
#include <stdint.h>
#include <X11/Xlib.h>
#include "JBWMPixels.h"
#include "JBWMSize.h"
struct JBWMScreen {
	Screen * xlib;
#ifdef JBWM_USE_EWMH
	Window supporting;
#endif//JBWM_USE_EWMH
	struct JBWMPixels pixels;
	uint8_t id, vdesk;
} __attribute__((packed));
#endif//!JBWM_JBWMSCREEN_H
