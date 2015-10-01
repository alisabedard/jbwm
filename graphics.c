/* Copyright 2008-2011, Jeffrey E. Bedard <antiright@gmail.com> */

#include "jbwm.h"

XColor
jbwm_get_XColor(const ubyte r, const ubyte g, const ubyte b)
{
	XColor c;

	c.red = r << 8;
	c.green = g << 8;
	c.blue = b << 8;

	XAllocColor(jbwm.X.dpy, DefaultColormap(jbwm.X.dpy, 
		DefaultScreen(jbwm.X.dpy)), &c);

	return c;
}

GC
jbwm_new_gc_for_XColor(XColor color)
{
	XGCValues values;

	values.foreground = color.pixel;

	return XCreateGC(jbwm.X.dpy, jbwm.X.screens->root, 
			GCForeground, &values);
}

#ifdef USE_GRADIENT
void
draw_gradient(Window win, GC gc, const int x, const int y,
	const unsigned int w, const unsigned int h)
{
	unsigned int i;
	Display *dpy = jbwm.X.dpy;
	XGCValues gv;

	XGetGCValues(dpy, gc, GCForeground, &gv);
	if(gv.foreground)
		gv.foreground=gv.foreground/2;
	for(i = 0; i <= h; i++)
	{
		GC lgc;
		const ubyte d = 8;

		gv.foreground += (i<=h/2)?d:-d;
		lgc=XCreateGC(dpy, win, GCForeground, &gv);
#ifdef DEBUG
		printf("fg: %d line: %d\n", (int)gv.foreground, i);
		fflush(stdout);
#endif /* DEBUG */
		XDrawLine(dpy, win, lgc, x, y + i, x + w, y + i);
		XFreeGC(dpy, lgc);
	}
}
#endif /* USE_GRADIENT */


