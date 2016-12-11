// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H
#include "JBWMScreen.h"
#include <stdbool.h>
#include <X11/Xutil.h>
#include "typedef.h"
struct JBWMClientOptions {
	bool fullscreen : 1;
	bool max_horz : 1;
	bool max_vert : 1;
	bool no_border : 1;
	bool no_close : 1;
	bool no_max : 1;
	bool no_min : 1;
	bool no_move : 1;
	bool no_resize : 1;
	bool no_title_bar : 1;
	bool remove : 1;
	bool shaded : 1;
	bool shaped : 1;
	bool sticky : 1;
	bool tearoff : 1;
	bool pad : 1;
} __attribute__((packed));
#ifdef JBWM_USE_TITLE_BAR
struct JBWMClientTitlebar {
	jbwm_window_t win, close, resize, shade, stick;
};
#endif//JBWM_USE_TITLE_BAR
struct JBWMRectangle {
	int16_t x, y;
	uint16_t width, height;
};
struct JBWMClient {
	struct JBWMClient * next;
#ifdef JBWM_USE_TITLE_BAR
	struct JBWMClientTitlebar tb;
#endif//JBWM_USE_TITLE_BAR
	jbwm_cmap_t cmap;
	jbwm_window_t window, parent;
	struct JBWMClientOptions opt;
	int8_t ignore_unmap:3;
	uint8_t border:1;
	uint8_t vdesk:4;
	uint8_t screen;
	struct JBWMRectangle size, old_size;
} __attribute__((packed));
#endif /* JBWM_CLIENT_STRUCT_H */
