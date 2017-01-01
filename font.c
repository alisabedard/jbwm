// Copyright 2017, Jeffrey E. Bedard
#include "font.h"
#include <stdlib.h>
#include <X11/Xft/Xft.h>
#include "config.h"
#include "jbwm.h"
static XftFont * jbwm_font;
void jbwm_open_font(Display * d)
{
	if (jbwm_font)
		return; // already created
	char * font = getenv(JBWM_ENV_FONT);
	jbwm_font = XftFontOpenName(d, DefaultScreen(d), font);
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

