// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_UTIL_H
#define JBWM_UTIL_H

unsigned long pixel(const uint8_t screen, const char * restrict name);

#if defined(EWMH) || defined(MWM)
void *get_property(Window w, Atom property,
	unsigned long * restrict num_items);
#endif//EWMH||MWM

#endif//!JBWM_UTIL_H
