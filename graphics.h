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
