// Copyright 2020, Jeffrey E. Bedard
#ifndef JBWM_DISPLAY_H
#define JBWM_DISPLAY_H
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_set_up_error_handler(struct JBWMClient ** head_client);
Display * jbwm_open_display(void);
#endif//!JBWM_DISPLAY_H
