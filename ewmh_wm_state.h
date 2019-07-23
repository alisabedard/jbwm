// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_EWMH_WM_STATE_H
#define JBWM_EWMH_WM_STATE_H
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_ewmh_handle_wm_state_changes(XClientMessageEvent * e,
    struct JBWMClient * restrict c);
#endif//!JBWM_EWMH_WM_STATE_H
