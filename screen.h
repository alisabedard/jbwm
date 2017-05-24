// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SCREEN_H
#define JBWM_SCREEN_H
#include "JBWMClient.h"
#include "JBWMScreen.h"
struct JBWMScreen * jbwm_get_screens(void);
inline struct JBWMScreen * jbwm_get_screen(struct JBWMClient * restrict c)
{
	return jbwm_get_screens() + c->screen;
}
inline Window jbwm_get_root(Display * d, struct JBWMClient * restrict c)
{
	return RootWindow(d, jbwm_get_screen(c)->id);
}
void jbwm_set_screens(struct JBWMScreen * restrict s);
#endif /* JBWM_SCREEN_H */
