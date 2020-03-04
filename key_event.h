// Copyright 2020, Jeffrey E. Bedard
#ifndef JBWM_KEY_EVENT_H
#define JBWM_KEY_EVENT_H
#include <X11/Xlib.h>
struct JBWMClient;
struct JBWMScreen;
void jbwm_handle_key_event(struct JBWMScreen *s, 
    struct JBWMClient * target, XKeyEvent * e);

#endif//!JBWM_KEY_EVENT_H
