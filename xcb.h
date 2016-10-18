// Copyright 2016, Jeffrey E. Bedard
#ifndef JB_XCB_H
#define JB_XCB_H
#include <stdbool.h>
#include <stdint.h>
#include <xcb/xcb.h>
typedef uint32_t pixel_t;
void jb_check_x(xcb_connection_t * x);
xcb_connection_t * jb_get_xcb_connection(const char * display, int * screen);
xcb_screen_t * jb_get_xcb_screen(xcb_connection_t * x);
pixel_t jb_get_pixel(xcb_connection_t * x, const xcb_colormap_t cmap,
	const char * color);
pixel_t jb_get_rgb_pixel(xcb_connection_t * x, const xcb_colormap_t cm,
	const int16_t r, const int16_t g, const int16_t b);
pixel_t jb_set_fg(xcb_connection_t * x, const xcb_gc_t gc, const pixel_t p);
pixel_t jb_set_bg(xcb_connection_t * x, const xcb_gc_t gc, const pixel_t p);
xcb_atom_t jb_get_atom(xcb_connection_t * x, const char * name);
bool jb_next_event_timed(xcb_connection_t * x,
	xcb_generic_event_t ** e, const uint32_t delay);
bool jb_xcb_cookie_has_error(xcb_connection_t * x,
	const xcb_void_cookie_t c);
#endif//!JB_XCB_H
