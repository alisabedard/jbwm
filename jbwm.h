// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_H
#define JBWM_H

#include "jbwmenv.h"

void jbwm_grab_button(const Window w, const unsigned int mask,
		      const unsigned int button);

unsigned long pixel(const uint8_t screen, const char * restrict name);

#endif /* not JBWM_JBWM_H */
