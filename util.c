// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "util.h"
#include "JBWMEnv.h"
void jbwm_set_property(const jbwm_window_t win,
	const jbwm_atom_t property, const jbwm_atom_t type,
	void * restrict data, int16_t size)
{
	XChangeProperty(jbwm.d, win, property, type, 32, PropModeReplace,
		data, size);
}
unsigned long jbwm_get_pixel(const uint8_t screen, const char * restrict name)
{
	if (!name) // sanitize input to avoid segfault
		return 0;
	XColor c, d;
	XAllocNamedColor(jbwm.d, DefaultColormap(jbwm.d, screen),
		name, &c, &d);
	return c.pixel;
}
#if defined(JBWM_USE_EWMH) || defined(MWM)
__attribute__((warn_unused_result))
void *jbwm_get_property(jbwm_window_t w, Atom property, uint16_t * num_items)
{
	unsigned char * prop;
	long unsigned int n;
	{
		long unsigned int b;
		int d;
		XGetWindowProperty(jbwm.d, w, property, 0, 1024, false,
			AnyPropertyType, &property, &d, &n, &b, &prop);
	}
	*num_items = n;
	return prop;
}
#endif//JBWM_USE_EWMH||MWM
void jbwm_grab_button(const jbwm_window_t w, const unsigned int mask,
		 const unsigned int btn)
{
	XGrabButton(jbwm.d, btn, mask, w, false,
		    ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
		    GrabModeSync, None, None);
}
#ifdef DEBUG
#include <stdio.h>
void jbwm_print_atom(const Atom a, const char * src, const uint16_t line)
{
	char *an = XGetAtomName(jbwm.d, a);
	fprintf(stderr, "\t%s:%d %s(%lu)\n", src, line, an, a);
	XFree(an);
}
#endif//DEBUG
