// Copyright 2016, Jeffrey E. Bedard
#include "font.h"
#include "config.h"
#include "jbwm.h"
#include <stdlib.h>
#ifdef JBWM_USE_XFT
#include <X11/Xft/Xft.h>
#endif//JBWM_USE_XFT
static struct {
#ifdef JBWM_USE_XFT
	XftFont * font;
#else//! JBWM_USE_XFT
	XFontStruct * font;
#endif//JBWM_USE_XFT
	uint8_t height;
} jbwm_font_data;
void jbwm_open_font(Display * restrict d)
{
	if (jbwm_font_data.font)
		return; // already created
	char * font = getenv(JBWM_ENV_FONT);
#ifdef JBWM_USE_XFT
	jbwm_font_data.font = XftFontOpen(d, DefaultScreen(d), XFT_FAMILY,
		XftTypeString, font, XFT_SIZE, XftTypeDouble,
		JBWM_FONT_SIZE, NULL);
#else//!JBWM_USE_XFT
	jbwm_font_data.font = XLoadQueryFont(d, font);
#endif//JBWM_USE_XFT
	if (!jbwm_font_data.font)
		jbwm_error(JBWM_ENV_FONT);
}
void * jbwm_get_font(void)
{
	return jbwm_font_data.font;
}
uint8_t jbwm_get_font_ascent(void)
{
	return jbwm_font_data.font->ascent;
}
uint8_t jbwm_get_font_height(void)
{
	return jbwm_font_data.font->ascent
		+ jbwm_font_data.font->descent;
}

