// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_MWM_H
#define JBWM_MWM_H

#ifdef MWM

#include "client_t.h"

void handle_mwm_hints(Client * c);

#else

#define handle_mwm_hints(c)

#endif

#endif//!JBWM_MWM_H
