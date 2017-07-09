// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_TITLE_BAR_H
#define JBWM_TITLE_BAR_H
struct JBWMClient;
void jbwm_toggle_shade(struct JBWMClient * restrict c);
void jbwm_update_title_bar(struct JBWMClient * restrict c);
#endif /* JBWM_TITLE_BAR_H */
