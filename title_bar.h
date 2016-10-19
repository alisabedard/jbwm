// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_TITLE_BAR_H
#define JBWM_TITLE_BAR_H
#ifdef USE_TBAR
#include "JBWMEnv.h"
void jbwm_toggle_shade(struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_update_title_bar(struct JBWMClient * c)
	__attribute__((nonnull));
/* Ensure that the title text fits within the title_bar.  */
#define TDIM (jbwm.font->ascent+jbwm.font->descent)
#else//!USE_TBAR
#define TDIM 0
#define jbwm_toggle_shade(c)
#define jbwm_update_title_bar(c)
#endif//USE_TBAR
#endif /* JBWM_TITLE_BAR_H */
