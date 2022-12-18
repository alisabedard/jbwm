// Copyright 2020, Alisa Bedard
#ifndef JBWM_KEY_EVENT_H
#define JBWM_KEY_EVENT_H
#include <X11/Xlib.h>
struct JBWMClient;
struct JBWMScreen;
void jbwm_handle_key_event(struct JBWMScreen *s, struct JBWMClient * target, 
  struct JBWMClient ** head_client, struct JBWMClient ** current_client,
  XKeyEvent * e);
#endif//!JBWM_KEY_EVENT_H
