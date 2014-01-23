#ifndef ARWM_GRAPHICS_H
#define ARWM_GRAPHICS_H

#include "ARWMButton.h"

GC
arwm_new_gc_for_XColor(XColor color);

XColor
arwm_get_XColor(const ubyte r, const ubyte g, const ubyte b);

/*GC
arwm_new_gc(const ubyte r, const ubyte g, const ubyte b);*/

#define arwm_new_gc(r, g, b) arwm_new_gc_for_XColor(arwm_get_XColor(r, g, b))

#ifdef USE_XBM
#define arwm_get_XImage_for_XBM(data, width, height)\
	XCreateImage(arwm.X.dpy, (Visual *) CopyFromParent, 1, XYBitmap,\
			0, (char *) data, width, height, 8, 0)
#endif /* USE_XBM */

#ifdef USE_GRADIENT
void
draw_gradient(Window win, GC gc, const int x, const int y,
	const unsigned int w, const unsigned int h);
#endif /* USE_GRADIENT */

#endif /* ARWM_GRAPHICS_H */

