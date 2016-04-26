// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <stdlib.h>
#include <X11/Xlib.h>
#include "jbwmenv.h"

unsigned long pixel(const uint8_t screen, const char * restrict name)
{
	XColor c;
	XAllocNamedColor(jbwm.dpy, DefaultColormap(jbwm.dpy, screen),
		name, &c, &(XColor){0});
	return c.pixel;
}

#if defined(EWMH) || defined(MWM)
__attribute__((warn_unused_result))
void *get_property(Window w, Atom property, unsigned long * restrict num_items)
{
	unsigned char *prop;
	// Recycle property as dummy variable.
	if (XGetWindowProperty(jbwm.dpy, w, property, 0L, 1024LL,
		false, AnyPropertyType, &property, (int*)&property, num_items,
		(unsigned long *)&property, &prop) == Success)
		return prop;
	return NULL;
}
#endif//EWMH||MWM


