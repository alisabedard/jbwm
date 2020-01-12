// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_EWMH_CLIENT_H
#define JBWM_EWMH_CLIENT_H
#include <X11/Xlib.h>
struct JBWMClient;
struct JBWMScreen;
void jbwm_ewmh_handle_client_message(XClientMessageEvent * e,
    struct JBWMClient * restrict c, struct JBWMScreen * s);
#endif//!JBWM_EWMH_CLIENT_H
