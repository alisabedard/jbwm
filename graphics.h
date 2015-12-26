#ifndef JBWM_GRAPHICS_H
#define JBWM_GRAPHICS_H

GC
jbwm_new_gc(XColor c);

XColor
jbwm_color(const char *name);

void
free_color(XColor c);

void
draw(Window w, XSizeHints *g, const char *color);

#ifdef USE_XPM
void
draw_xpm(Window w, XSizeHints *g, char **xpm);
#endif /* USE_XPM */

#endif /* JBWM_GRAPHICS_H */
