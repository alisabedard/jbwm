// Copyright 2020, Jeffrey E. Bedard
#include "font.h"
#ifdef JBWM_USE_XFT
#include <X11/Xft/Xft.h>
#endif//JBWM_USE_XFT
#include "config.h"
#include <stdbool.h>
#include "util.h"
static struct JBWMFont {
#ifdef JBWM_USE_XFT
  XftFont * font;
#else//!JBWM_USE_XFT
  XFontStruct * font;
#endif//JBWM_USE_XFT
  short ascent,descent,height;
} jbwm_font;
void jbwm_open_font(Display * d)
{
  static bool already_created;
  if(!already_created){
#ifdef JBWM_USE_XFT
    jbwm_font.font = XftFontOpenName(d, DefaultScreen(d), JBWM_FONT);
#else//!JBWM_USE_XFT
    jbwm_font.font = XLoadQueryFont(d, JBWM_FONT);
#endif//JBWM_USE_XFT
    if (!jbwm_font.font)
      jbwm_error(JBWM_FONT);
#ifdef JBWM_USE_XFT
    jbwm_font.ascent=jbwm_font.font->ascent;
    jbwm_font.descent=jbwm_font.font->descent;
#else//!JBWM_USE_XFT
    jbwm_font.ascent=jbwm_font.font->max_bounds.ascent;
    jbwm_font.descent=jbwm_font.font->max_bounds.descent;
#endif//JBWM_USE_XFT
    jbwm_font.height=jbwm_font.ascent+jbwm_font.descent;
    already_created=true;
  }
}
void * jbwm_get_font(void)
{
  return jbwm_font.font;
}
uint8_t jbwm_get_font_ascent(void)
{
  return jbwm_font.ascent;
}
uint8_t jbwm_get_font_height(void)
{
  return jbwm_font.height;
}
