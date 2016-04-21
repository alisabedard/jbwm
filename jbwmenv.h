// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_ENVIRONMENT_H
#define JBWM_ENVIRONMENT_H

#include <stdbool.h>
#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif//USE_XFT
#include "client_t.h"

typedef struct {
	bool need_cleanup;
	struct {
		uint16_t grab, mod;
	} keymasks;
	Cursor cursor;
	Display *restrict dpy;
	// Client tracking:
	Client *current;
	Client *head;
	ScreenInfo *screens;
#ifdef USE_TBAR
#ifdef USE_XFT
	XftFont *font;
#else//! USE_XFT
	XFontStruct *font;
#endif//USE_XFT
#endif//USE_TBAR

#ifdef USE_TBAR
	struct {
		GC close, shade, resize;
	} gc;
#endif//USE_TBAR
} JBWMEnvironment;

extern JBWMEnvironment jbwm;

#endif /* not JBWM_ENVIRONMENT_H */
