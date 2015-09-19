#include "jbwm.h"

void
ARWMButton_delete(ARWMButton * button)
{
	XFreeGC(jbwm.X.dpy, button->gc);
	free(button);
}

void
ARWMButton_draw(ARWMButton * button)
{
	XRectangle *g = &(button->geometry);

#ifdef GRAPHICS_DEBUG
	LOG_DEBUG("ARWMButton_draw()\n");
#endif /* GRAPHICS_DEBUG */


#ifndef USE_XPM
#ifdef USE_GRADIENT
	draw_gradient(button->parent, button->gc, g->x, g->y, 
		g->width, g->height);
#else /* !USE_GRADIENT */
#ifndef USE_XBM
	XFillRectangle(jbwm.X.dpy, button->parent, button->gc, 
		g->x, g->y, g->width, g->height);
#endif
#endif /* USE_GRADIENT */
#endif /* ! USE_XPM */


#if defined(USE_XPM) || defined(USE_XBM)
	if(button->image)
	{

		Window w = button->parent;
		int image_x = g->x, image_y = g->y;
		const unsigned short width = g->width;

#ifdef GRAPHICS_DEBUG
		LOG_DEBUG("\tXPutImage()\n");
#endif /* GRAPHICS_DEBUG */
#define POS_IMG(wh, xy) if(button->image_##wh) {\
	image_##xy +=(button->geometry.wh - button->image_##wh)/2; }
		POS_IMG(width, x);
		POS_IMG(height, y);
		if(button->span_image)
		{
			XWindowAttributes wa;
			unsigned short i;

			const ubyte inc = 24;

			XGetWindowAttributes(jbwm.X.dpy, w, &wa);
			for(i=0; i <(unsigned int)wa.width; i+=inc)
			{
#ifdef DEBUG
				printf("i:%d\t<\t", i);
				printf("wa.width:%d\t\n", width);
#endif /* DEBUG */
				XPutImage(jbwm.X.dpy, w, button->gc, 
						button->image, 0, 0, image_x+i
#ifdef USE_XBM
					+1
#endif /* USE_XBM */
					, 
#ifdef USE_XBM
					1,
#else
					image_y, 
#endif /* USE_XBM */
					width, g->height);
			}
		}
		else
XPutImage(jbwm.X.dpy, w, button->gc, 
						button->image, 0, 0, image_x
#ifdef USE_XBM
					+1
#endif /* USE_XBM */
					, 
#ifdef USE_XBM
					1,
#else
					image_y, 
#endif /* USE_XBM */
					width, g->height);

	}
#endif /* USE_XPM || USE_XBM */
}

ARWMButton *
ARWMButton_new(Window parent, GC gc, const unsigned short width,
	const unsigned short height , void *image
#if !defined(USE_XPM) || !defined(USE_XBM)
	__attribute((unused))
#endif
	)
{
	ARWMButton *button;

#ifdef GRAPHICS_DEBUG
	LOG_DEBUG("new_ARWMButton()\n");
#endif /* GRAPHICS_DEBUG */

	button = calloc(1, sizeof(ARWMButton));
	button->parent = parent;
	button->gc = gc;
	button->geometry.width = width;
	button->geometry.height = height;

#if defined(USE_XPM) || defined(USE_XBM)
	button->image = image;
#else
	(void)image;
#endif /* USE_XPM || USE_XBM */

	return button;
}
