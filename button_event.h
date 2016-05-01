// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_BTN_EVT_H
#define JBWM_BTN_EVT_H

#include "client_t.h"

void jbwm_handle_button_event(XButtonEvent * restrict e,
	Client * restrict c) __attribute__((nonnull(1)));

#endif /* JBWM_BTN_EVT_H */
