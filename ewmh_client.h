// Copyright 2020, Jeffrey E. Bedard
#ifndef JBWM_EWMH_CLIENT_H
#define JBWM_EWMH_CLIENT_H
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_ewmh_handle_client_message(XClientMessageEvent * e,
    struct JBWMClient * c);
#endif//!JBWM_EWMH_CLIENT_H
