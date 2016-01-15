// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

XColor
jbwm_color(const char *name)
{
	XColor c, none;
	const uint8_t s=DefaultScreen(D);
	XAllocNamedColor(D, DefaultColormap(D, s), name, &c, &none);
	return c;
}

