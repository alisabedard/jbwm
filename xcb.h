// Copyright 2016, Jeffrey E. Bedard
#ifndef JB_XCB_H
#define JB_XCB_H
#include <xcb/xcb.h>

xcb_connection_t * jb_xcb_connect(int * screen);
xcb_screen_t * jb_get_xcb_screen(xcb_connection_t * x);

#endif//!JB_XCB_H
