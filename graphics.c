// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

#define S DefaultScreen(D)
#define CM DefaultColormap(D, S)
void
free_color(XColor *c)
{
	XFreeColors(D, CM, &c->pixel, 1, AllPlanes);
}

XColor
jbwm_color(const char *name)
{
	XColor c, none;
	XAllocNamedColor(D, CM, name, &c, &none);
	return c;
}

GC
jbwm_new_gc(XColor *color)
{
	XGCValues values={.foreground=color->pixel};
	return XCreateGC(D, jbwm.X.screens->root, GCForeground, &values);
}

void
draw_rectangle(Window w, XRectangle *g, const char *color)
{
	XColor c=jbwm_color(color);
	GC gc=jbwm_new_gc(&c);
	XFillRectangle(D, w, gc, g->x, g->y, g->width, g->height);
	XFreeGC(D, gc);
	free_color(&c);
}

