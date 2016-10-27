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
};
extern struct JBWMEnv jbwm;
#ifdef JBWM_USE_TITLE_BAR
void * jbwm_get_font(void);
uint8_t jbwm_get_font_ascent(void);
uint8_t jbwm_get_font_descent(void);
uint8_t jbwm_get_font_height(void);
#else//!JBWM_USE_TITLE_BAR
#define jbwm_get_font_height() 0
#endif//JBWM_USE_TITLE_BAR
struct JBWMScreen * jbwm_get_screens(void);
#endif /* not JBWM_ENVIRONMENT_H */
