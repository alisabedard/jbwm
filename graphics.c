/* Copyright 2008-2015, Jeffrey E. Bedard <jefbed@gmail.com> */

#include "jbwm.h"

void
free_color(XColor c)
{
        Display *d;
        unsigned long p[1];

        p[0]=c.pixel;
        d=jbwm.X.dpy;
	{
        	const Colormap cm=DefaultColormap(d, DefaultScreen(d));

        	XFreeColors(d, cm, p, 1, AllPlanes);
	}
}

XColor
jbwm_color(const char *name)
{
	XColor c, none;
	Display *d;

	d=jbwm.X.dpy;
	XAllocNamedColor(d, DefaultColormap(d, DefaultScreen(d)), name, &c, 
		&none);

	return c;
}

GC
jbwm_new_gc(XColor color)
{
	XGCValues values;

	values.foreground = color.pixel;

	return XCreateGC(jbwm.X.dpy, jbwm.X.screens->root,
		GCForeground, &values);
}

void
draw(Window w, XRectangle *g, const char *color)
{
	GC gc;
	Display *d;
	XColor c;
	

	c=jbwm_color(color);
	gc=jbwm_new_gc(c);
	d=jbwm.X.dpy;
	XFillRectangle(d, w, gc, g->x, g->y, g->width, g->height);
	XFreeGC(d, gc);
	free_color(c);
}

#ifdef USE_XPM
#include <X11/xpm.h>

void
draw_xpm(Window w, XRectangle *g, char **xpm)
{
	XImage *i;
	Display *d;
	GC gc;
	XpmAttributes a;

	d=jbwm.X.dpy;
	a.valuemask=XpmSize;
	XpmCreateImageFromData(d, xpm, &i, NULL, &a);
	gc=XCreateGC(d, w, 0, NULL);
	XPutImage(d, w, gc, i, 0, 0, g->x, g->y, a.width, a.height);
	XFreeGC(d, gc);
	XDestroyImage(i);
}
#endif /* USE_XPM */
