// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <assert.h>
#include <stdbool.h>
#include <X11/extensions/shape.h>
#include "client_t.h"
#include "jbwmenv.h"
#include "log.h"

static bool is_shaped(Client * c)
{
	assert(c);
	int d, s;
#define U (unsigned int *)
	return XShapeQueryExtents(jbwm.dpy, c->window, &s, &d, &d, 
		U & d, U & d, &d, &d, &d, U & d, U & d) && s;
}

void set_shape(Client * restrict c)
{
	assert(c);
	if(c->flags & JB_SHAPED)
		XShapeCombineShape(jbwm.dpy, c->parent, ShapeBounding,
			0, 0, c->window, ShapeBounding, ShapeSet);
}

void setup_shaped(Client * restrict c)
{
	assert(c);
	if (is_shaped(c)) {
		LOG("Window %d is shaped", (int)c->window);
		c->border = 0;
		c->flags |= JB_NO_TB | JB_SHAPED;
	}
}

