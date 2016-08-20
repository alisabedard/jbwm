// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "util.h"

#include "JBWMEnv.h"

#include <unistd.h>

__attribute__((nonnull(2)))
unsigned long pixel(const uint8_t screen, const char * restrict name)
{
	XColor c;
	XAllocNamedColor(jbwm.d, DefaultColormap(jbwm.d, screen),
		name, &c, &(XColor){});
	return c.pixel;
}

#if defined(EWMH) || defined(MWM)
__attribute__((warn_unused_result))
void *get_property(Window w, Atom property, uint16_t * num_items)
{
	unsigned char * prop;
	long unsigned int n, b;
	int d;
	XGetWindowProperty(jbwm.d, w, property, 0, 1024, false,
		AnyPropertyType, &property, &d, &n, &b, &prop);
	*num_items = n;
	return prop;
}
#endif//EWMH||MWM

void jbwm_grab_button(const Window w, const unsigned int mask,
		 const unsigned int btn)
{
	XGrabButton(jbwm.d, btn, mask, w, false,
		    ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
		    GrabModeSync, None, None);
}

#ifdef DEBUG
#include <stdio.h>
void print_atom(const Atom a, const char * src, const uint16_t line)
{
	char *an = XGetAtomName(jbwm.d, a);
	fprintf(stderr, "\t%s:%d %s(%lu)\n", src, line, an, a);
	XFree(an);
}
#endif//DEBUG


