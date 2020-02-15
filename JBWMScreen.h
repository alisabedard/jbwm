// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_JBWMSCREEN_H
#define JBWM_JBWMSCREEN_H
#include "JBWMPixels.h"
#include <X11/Xlib.h>
#ifdef JBWM_USE_XFT
#include <X11/Xft/Xft.h>
#endif//JBWM_USE_XFT
struct JBWMScreen {
    Display *display;
    Screen * xlib;
#ifdef JBWM_USE_XFT
    XftDraw * xft;
    XftColor font_color;
#else//!JBWM_USE_XFT
    XFontStruct *font;
#endif//JBWM_USE_XFT
    Window supporting;
    struct JBWMPixels pixels;
    GC gc,border_gc;
    uint8_t id, vdesk, font_height;
};
#endif//!JBWM_JBWMSCREEN_H
