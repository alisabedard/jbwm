// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "screen.h"
extern inline Window jbwm_get_root(Display * d, struct JBWMClient * restrict
	c);
extern inline struct JBWMScreen * jbwm_get_screen(struct JBWMClient * restrict
	c);
static struct JBWMScreen * screens;
struct JBWMScreen * jbwm_get_screens(void)
{
	return screens;
}
void jbwm_set_screens(struct JBWMScreen * restrict s)
{
	screens = s;
}
