// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "screen.h"
extern inline Window jbwm_get_client_root(struct JBWMClient * c);
extern inline struct JBWMScreen * jbwm_get_screen(struct JBWMClient * c);
static struct JBWMScreen * screens;
struct JBWMScreen * jbwm_get_screens(void)
{
	return screens;
}
void jbwm_set_screens(struct JBWMScreen * restrict s)
{
	screens = s;
}
Window jbwm_get_current_root(Display * dpy)
{
	Window w = DefaultRootWindow(dpy), r;
	int i;
	unsigned int ui;
	XQueryPointer(dpy, w, &r, &w, &i, &i, &i, &i, &ui);
	return r;
}
Screen * jbwm_get_current_xlib_screen(Display * dpy)
{
	XWindowAttributes a;
	XGetWindowAttributes(dpy, jbwm_get_current_root(dpy), &a);
	return a.screen;
}
