// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "shape.h"

#include "jbwmenv.h"
#include "log.h"

#include <X11/extensions/shape.h>

// Declared with pure attribute, as value may not change between calls.
__attribute__((pure))
static bool is_shaped(Client * c)
{
	int s, d;
	unsigned int u;
	return XShapeQueryExtents(jbwm.d, c->window, &s, &d, &d,
		&u, &u, &d, &d, &d, &u, &u) && s;
}

void set_shape(Client * c)
{
	if(c->opt.shaped) {
		LOG("XShapeCombineShape: %d", (int)c->window);
		XShapeCombineShape(jbwm.d, c->parent, ShapeBounding,
			1, 1, c->window, ShapeBounding, ShapeSet);
	}
}

void setup_shaped(Client * c)
{
	if (is_shaped(c)) {
		LOG("Window %d is shaped", (int)c->window);
		c->border = 0;
		c->opt.no_titlebar=c->opt.shaped=true;
	}
}

