// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_GRAPHICS_H
#define JBWM_GRAPHICS_H

GC
jbwm_new_gc(XColor *c);

XColor
jbwm_color(const char *name);

void
free_color(XColor *c);

void
draw_rectangle(Window w, XRectangle *g, const char *color);

#endif /* JBWM_GRAPHICS_H */
