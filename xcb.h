// Copyright 2016, Jeffrey E. Bedard
#ifndef JB_XCB_H
#define JB_XCB_H

#include <stdint.h>
#include <xcb/xcb.h>

typedef uint32_t pixel_t;

xcb_connection_t * jb_get_xcb_connection(const char * display, int * screen);

xcb_screen_t * jb_get_xcb_screen(xcb_connection_t * x);

pixel_t jb_get_pixel(xcb_connection_t * x, const xcb_colormap_t cmap,
	const char * color);

#endif//!JB_XCB_H
