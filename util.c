// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "util.h"

#include "jbwmenv.h"

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
void *get_property(jbwm_window_t w, jbwm_atom_t property,
	uint16_t * restrict num_items)
{
	unsigned char *prop;
	return (XGetWindowProperty(jbwm.d, w, property, 0L, 1024LL,
		false, AnyPropertyType, &property, &(int){0},
		(long unsigned int *)num_items,
		&(unsigned long){0}, &prop) == Success) ? prop : NULL;
}
#endif//EWMH||MWM

void jbwm_grab_button(const jbwm_window_t w, const uint32_t mask,
	const uint32_t btn)
{
	XGrabButton(jbwm.d, btn, mask, w, false,
		    ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
		    GrabModeSync, None, None);
}

#ifdef DEBUG
#include <stdio.h>
void print_atom(const jbwm_atom_t a, const char * src, const uint16_t line)
{
	char *an = XGetAtomName(jbwm.d, a);
	fprintf(stderr, "\t%s:%d %s(%lu)\n", src, line, an, a);
	XFree(an);
}
#endif//DEBUG


