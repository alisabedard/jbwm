// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_TITLEBAR_H
#define JBWM_TITLEBAR_H
#ifdef USE_TBAR
#include "JBWMEnv.h"
void jbwm_toggle_shade(struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_update_titlebar(struct JBWMClient * c)
	__attribute__((nonnull));
/* Ensure that the title text fits within the titlebar.  */
#define TDIM (jbwm.font->ascent+jbwm.font->descent)
#else//!USE_TBAR
#define TDIM 0
#define jbwm_toggle_shade(c)
#define jbwm_update_titlebar(c)
#endif//USE_TBAR
#endif /* JBWM_TITLEBAR_H */
