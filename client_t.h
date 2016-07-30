// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H

#include <stdbool.h>
#include <X11/Xutil.h>
#include "ScreenInfo.h"

typedef struct Client Client;

struct Client {
	XRectangle old_size;
	XSizeHints size;
	jbwm_window_t window, parent;
#ifdef USE_TBAR
	jbwm_window_t titlebar;
#endif//USE_TBAR
	jbwm_cmap_t cmap;
	ScreenInfo *screen;
	Client *next;
	int8_t ignore_unmap;
	uint8_t vdesk : 4;
	uint8_t border : 1;
#ifdef USE_TBAR
	uint16_t exposed_width;
#endif//USE_TBAR
	struct {
		bool sticky : 1;
		bool active : 1;
		bool remove : 1;
		bool fullscreen : 1;
		bool shaded : 1;
		bool shaped : 1;
		bool is_fullscreen : 1;
		bool no_titlebar : 1;
		bool no_min : 1;
		bool no_resize : 1;
		bool no_max : 1;
		bool no_move : 1;
		bool no_border : 1;
		bool no_close : 1;
		bool no_resize_decor : 1;
		bool no_close_decor : 1;
		bool no_min_decor : 1;
		//bool no_max_decor : 1;
		bool max_horz : 1;
		bool max_vert : 1;
		//bool modal : 1;
		bool tearoff : 1;
	} opt;
};

#endif /* JBWM_CLIENT_STRUCT_H */
