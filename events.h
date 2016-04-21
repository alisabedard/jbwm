// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_EVENTS_H
#define JBWM_EVENTS_H

#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include "client_t.h"

#ifdef DEBUG
void print_atom(const Atom a, const unsigned int line);
#endif//DEBUG

void main_event_loop(void);

void jbwm_current_to_head(void);

void do_window_changes(int value_mask, XWindowChanges * restrict wc, 
	Client * restrict c, int gravity);

#endif /* ! JBWM_EVENTS_H */
