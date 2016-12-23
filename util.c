// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "util.h"
#include "jbwm.h"
#include <stdbool.h>
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
	if (!name) // sanitize input to avoid segfault
		return 0;
	XColor c, d;
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen),
		name, &c, &d);
	return c.pixel;
}
__attribute__((warn_unused_result,nonnull))
void *jbwm_get_property(Display * dpy, Window w,
	Atom property, uint16_t * num_items)
{
	unsigned char * prop;
	long unsigned int n;
	{
		long unsigned int b;
		int d;
		if (XGetWindowProperty(dpy, w, property, 0, 1024, false,
			AnyPropertyType, &property, &d, &n, &b, &prop)
			!= Success) {
			*num_items = 0;
			return NULL;
		}
	}
	*num_items = n;
	return prop;
}
void jbwm_grab_button(Display * d, const Window w,
	const unsigned int mask, const unsigned int btn)
{
	XGrabButton(d, btn, mask, w, false,
		    ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
		    GrabModeSync, None, None);
}
#ifdef DEBUG
#include <stdio.h>
void jbwm_print_atom(Display * d, const Atom a, const char * src,
	const uint16_t line)
{
	char *an = XGetAtomName(d, a);
	fprintf(stderr, "\t%s:%d %s(%lu)\n", src, line, an, a);
	XFree(an);
}
#endif//DEBUG
