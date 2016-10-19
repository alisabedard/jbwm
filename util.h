// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_UTIL_H
#define JBWM_UTIL_H
#include "typedef.h"
void jbwm_set_property(const jbwm_window_t win,
	const jbwm_atom_t property, const jbwm_atom_t type,
	void * restrict data, int16_t size);
unsigned long jbwm_get_pixel(const uint8_t screen,
	const char * restrict name);
#if defined(EWMH) || defined(MWM)
// Free the result with XFree
void * jbwm_get_property(const jbwm_window_t w,
	Atom property, uint16_t * num_items);
#endif//EWMH||MWM
void jbwm_grab_button(const jbwm_window_t w, const unsigned int mask,
		 const unsigned int btn);
#ifdef DEBUG
void jbwm_print_atom(const Atom a, const char * src, const uint16_t line);
#else//!DEBUG
#define jbwm_print_atom(a, s, l)
#endif//DEBUG
#endif//!JBWM_UTIL_H
