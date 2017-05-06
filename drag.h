// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef JBWM_DRAG_H
#define JBWM_DRAG_H
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdint.h>
struct JBWMClient;
/* Drag the specified client.  Resize the client if resize is true.  */
void jbwm_drag(Display * d, struct JBWMClient * restrict c,
	const bool resize) __attribute__((nonnull));
/* Warp the pointer to the specified position.  */
void jbwm_warp(Display * dpy, const Window w, const int16_t x,
	const int16_t y);
#endif//!JBWM_DRAG_H
