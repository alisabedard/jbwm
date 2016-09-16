// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_ENVIRONMENT_H
#define JBWM_ENVIRONMENT_H

#include "JBWMClient.h"

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif//USE_XFT

struct JBWMEnv {
	Display * d;
	// Client tracking:
	struct JBWMClient * current, * head;
	struct JBWMScreen * s;
#ifdef USE_TBAR
#ifdef USE_XFT
	XftFont * font;
#else//! USE_XFT
	XFontStruct * font;
#endif//USE_XFT
#endif//USE_TBAR
	jbwm_window_t last;
	struct {
		uint16_t grab, mod;
	} keymasks;
	Cursor cursor;
	bool need_cleanup:1;
};

extern struct JBWMEnv jbwm;

#endif /* not JBWM_ENVIRONMENT_H */
