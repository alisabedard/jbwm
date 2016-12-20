// Copyright 2016, Jeffrey E. Bedard
#include "font.h"
#include <stdlib.h>
#ifdef JBWM_USE_XFT
#include <X11/Xft/Xft.h>
#endif//JBWM_USE_XFT
#include "config.h"
#include "jbwm.h"
#ifdef JBWM_USE_XFT
static XftFont * jbwm_font;
#else//! JBWM_USE_XFT
static XFontStruct * jbwm_font;
#endif//JBWM_USE_XFT
void jbwm_open_font(Display * d)
{
	if (jbwm_font)
		return; // already created
	char * font = getenv(JBWM_ENV_FONT);
#ifdef JBWM_USE_XFT
	jbwm_font = XftFontOpenName(d, DefaultScreen(d), font);
#else//!JBWM_USE_XFT
	jbwm_font = XLoadQueryFont(d, font);
#endif//JBWM_USE_XFT
	if (!jbwm_font)
		jbwm_error(JBWM_ENV_FONT);
}
void * jbwm_get_font(void)
{
	return jbwm_font;
}
uint8_t jbwm_get_font_ascent(void)
{
	return jbwm_font->ascent;
}
uint8_t jbwm_get_font_height(void)
{
	return jbwm_font->ascent + jbwm_font->descent;
}
uint8_t jbwm_get_font_width(void)
{
	return jbwm_font->max_advance_width;
}

