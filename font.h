// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_FONT_H
#define JBWM_FONT_H
#include <stdint.h>
#include <X11/Xlib.h>
void * jbwm_get_font(void);
uint8_t jbwm_get_font_ascent(void);
uint8_t jbwm_get_font_height(void);
void jbwm_open_font(Display * d);
#endif//JBWM_FONT_H
