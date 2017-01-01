// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_FONT_H
#define JBWM_FONT_H
#ifdef JBWM_USE_TITLE_BAR
#include <stdint.h>
#include <X11/Xlib.h>
void * jbwm_get_font(void);
uint8_t jbwm_get_font_ascent(void);
uint8_t jbwm_get_font_height(void);
uint8_t jbwm_get_font_width(void);
void jbwm_open_font(Display * d);
#else//!JBWM_USE_TITLE_BAR
#define jbwm_get_font_height() 0
#define jbwm_get_font_width() 0
#define jbwm_open_font(d)
#endif//JBWM_USE_TITLE_BAR
#endif//JBWM_FONT_H
