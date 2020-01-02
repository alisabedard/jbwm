// Copyright 2017, Jeffrey E. Bedard
#include "font.h"
#include <X11/Xft/Xft.h>
#include "config.h"
#include "util.h"
static XftFont * jbwm_font;
void jbwm_open_font(Display * d)
{
    if (jbwm_font)
        return; // already created
    jbwm_font = XftFontOpenName(d, DefaultScreen(d), JBWM_FONT);
    if (!jbwm_font)
        jbwm_error(JBWM_FONT);
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
