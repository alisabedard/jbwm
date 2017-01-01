// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H
#include <stdbool.h>
#include <stdint.h>
#include <X11/Xutil.h>
#include "JBWMClientOptions.h"
#include "JBWMRectangle.h"
struct JBWMScreen;
#ifdef JBWM_USE_TITLE_BAR
struct JBWMClientTitlebar {
	Window win, close, resize, shade, stick;
};
#endif//JBWM_USE_TITLE_BAR
struct JBWMClient {
	struct JBWMClient * next;
#ifdef JBWM_USE_TITLE_BAR
	struct JBWMClientTitlebar tb;
#endif//JBWM_USE_TITLE_BAR
	Colormap cmap;
	Window window, parent;
	struct JBWMClientOptions opt;
	int8_t ignore_unmap:3;
	uint8_t border:1;
	uint8_t vdesk:4;
	uint8_t screen;
	struct JBWMRectangle size, old_size;
} __attribute__((packed));
#endif /* JBWM_CLIENT_STRUCT_H */
