// Copyright 2016, Jeffrey E. Bedard
#ifndef JBWM_DISPLAY_H
#define JBWM_DISPLAY_H
#include <X11/Xlib.h>
Display * jbwm_get_display(void);
void jbwm_open_display(void);
#endif//!JBWM_DISPLAY_H
