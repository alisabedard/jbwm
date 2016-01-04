/* Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> */

#include "jbwm.h"

#define S DefaultScreen(D)
#define CM DefaultColormap(D, S)
void
free_color(XColor c)
{
	XFreeColors(D, CM, &c.pixel, 1, AllPlanes);
}

XColor
jbwm_color(const char *name)
{
	XColor c, none;
	XAllocNamedColor(D, CM, name, &c, &none);
	return c;
}

GC
jbwm_new_gc(XColor color)
{
	XGCValues values={.foreground=color.pixel};
	return XCreateGC(D, jbwm.X.screens->root, GCForeground, &values);
}

void
draw(Window w, XRectangle *g, const char *color)
{
	XColor c=jbwm_color(color);
	GC gc=jbwm_new_gc(c);
	XFillRectangle(D, w, gc, g->x, g->y, g->width, g->height);
	XFreeGC(D, gc);
	free_color(c);
}

#ifdef USE_XPM
#include <X11/xpm.h>

void
draw_xpm(Window w, XRectangle *g, char **xpm)
{
	XImage *i;
	GC gc;
	XpmAttributes a;
	a.valuemask=XpmSize;
	XpmCreateImageFromData(D, xpm, &i, NULL, &a);
	gc=XCreateGC(D, w, 0, NULL);
	XPutImage(D, w, gc, i, 0, 0, g->x, g->y, a.width, a.height);
	XFreeGC(D, gc);
	XDestroyImage(i);
}
#endif /* USE_XPM */
