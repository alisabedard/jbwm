// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_TITLE_BAR_H
#define JBWM_TITLE_BAR_H
#ifdef JBWM_USE_TITLE_BAR
#include "JBWMClient.h"
void jbwm_toggle_shade(Display * restrict d, struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_update_title_bar(Display * restrict d, struct JBWMClient * c)
	__attribute__((nonnull));
#else//!JBWM_USE_TITLE_BAR
#define jbwm_toggle_shade(d, c)
#define jbwm_update_title_bar(d, c)
#endif//JBWM_USE_TITLE_BAR
#endif /* JBWM_TITLE_BAR_H */
