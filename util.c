// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
void jbwm_error(const char * restrict msg)
{
	perror(msg);
	exit(1);
}
void jbwm_set_property(Display * d, const Window win,
	const Atom property, const Atom type,
	void * data, uint16_t size)
{
	if (!d || !data)
		return;
	XChangeProperty(d, win, property, type, 32, PropModeReplace,
		data, size);
}
jbwm_pixel_t jbwm_get_pixel(Display * dpy,
	const uint8_t screen, const char * name)
{
	jbwm_pixel_t r;
	if (name) { // sanitize input to avoid segfault
		XColor c, d;
		XAllocNamedColor(dpy, DefaultColormap(dpy, screen),
			name, &c, &d);
		r = c.pixel;
	} else
		r = 0;
	return r;
}
void *jbwm_get_property(Display * dpy, Window w,
	Atom property, uint16_t * num_items)
{
	unsigned char * value;
	long unsigned int n;
	XGetWindowProperty(dpy, w, property, 0, 1024, false,
		AnyPropertyType, &property, &(int){0}, &n,
		&(unsigned long){0}, &value);
	if (property == None) {
		// Requested property not found
		XFree(value);
		value = NULL;
	}
	*num_items = n;
	return value;
}
void jbwm_grab_button(Display * d, const Window w,
	const unsigned int mask)
{
	XGrabButton(d, AnyButton, mask, w, false,
		    ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
		    GrabModeSync, None, None);
}
#ifdef DEBUG
void jbwm_print_atom(Display * d, const Atom a, const char * src,
	const uint16_t line)
{
	char *an = XGetAtomName(d, a);
	fprintf(stderr, "\t%s:%d %s(%lu)\n", src, line, an, a);
	XFree(an);
}
#endif//DEBUG
