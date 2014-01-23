/* Copyright 2008-2011, Jeffrey E. Bedard <antiright@gmail.com> */

#include "arwm.h"

XColor
arwm_get_XColor(const ubyte r, const ubyte g, const ubyte b)
{
	XColor color;

	color.red = r << 8;
	color.green = g << 8;
	color.blue = b << 8;

	XAllocColor(arwm.X.dpy, DefaultColormap(arwm.X.dpy, 
		DefaultScreen(arwm.X.dpy)), &color);

	return color;
}

GC
arwm_new_gc_for_XColor(XColor color)
{
	XGCValues values;

	values.foreground = color.pixel;

	return XCreateGC(arwm.X.dpy, arwm.X.screens->root, 
			GCForeground, &values);
}

#ifdef USE_GRADIENT
void
draw_gradient(Window win, GC gc, const int x, const int y,
	const unsigned int w, const unsigned int h)
{
	unsigned int i;
	Display *dpy = arwm.X.dpy;
	XGCValues gv;

	XGetGCValues(dpy, gc, GCForeground, &gv);

	for(i = 0; i <= h; i++)
	{
		GC lgc;
		const ubyte d = 7;

		gv.foreground += (i<=h/2)?d:-d;
		lgc=XCreateGC(dpy, win, GCForeground, &gv);
		XDrawLine(dpy, win, lgc, x, y + i, x + w, y + i);
		XFreeGC(dpy, lgc);
	}
}
#endif /* USE_GRADIENT */


