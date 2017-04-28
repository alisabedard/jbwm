// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_JBWMPIXELS_H
#define JBWM_JBWMPIXELS_H
#include "pixel_t.h"
struct JBWMPixels {
	jbwm_pixel_t fg, bg, fc;
#ifdef JBWM_USE_TITLE_BAR
	jbwm_pixel_t close, resize, shade, stick;
#endif//JBWM_USE_TITLE_BAR
};
#endif//!JBWM_JBWMPIXELS_H
