// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_NEW_H
#define JBWM_NEW_H

#include "JBWMScreen.h"

void jbwm_new_client(const jbwm_window_t w, struct JBWMScreen * restrict s)
	__attribute__((nonnull(2)));

#endif//!JBWM_NEW_H
