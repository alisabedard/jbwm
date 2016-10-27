// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_ENVIRONMENT_H
#define JBWM_ENVIRONMENT_H
#include "JBWMClient.h"
#ifdef JBWM_USE_XFT
#include <X11/Xft/Xft.h>
#endif//JBWM_USE_XFT
struct JBWMEnv {
	Display * d;
	// Client tracking:
	struct JBWMClient * current, * head;
	struct JBWMScreen * s;
#ifdef JBWM_USE_TITLE_BAR
#ifdef JBWM_USE_XFT
	XftFont * font;
#else//! JBWM_USE_XFT
	XFontStruct * font;
#endif//JBWM_USE_XFT
#endif//JBWM_USE_TITLE_BAR
};
extern struct JBWMEnv jbwm;
#endif /* not JBWM_ENVIRONMENT_H */
