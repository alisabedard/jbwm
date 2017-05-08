// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_EWMH_CLIENT_H
#define JBWM_EWMH_CLIENT_H
#ifdef JBWM_USE_EWMH
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_ewmh_handle_client_message(XClientMessageEvent * e,
	struct JBWMClient * restrict c);
#else//!JBWM_USE_EWMH
#define jbwm_ewmh_handle_client_message(e, c)
#endif//JBWM_USE_EWMH
#endif//!JBWM_EWMH_CLIENT_H
