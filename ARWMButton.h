#ifndef ARWMBUTTON_H
#define ARWMBUTTON_H

typedef struct _ARWMButton
{
	Window parent;
	XColor color;
	GC gc;
	XRectangle geometry;
	Bool span_image;

#if defined(USE_XPM) || defined(USE_XBM)
	XImage *image;
	unsigned int image_width, image_height;
#endif				/* USE_XPM || USE_XBM */
} ARWMButton;

ARWMButton *ARWMButton_new(Window parent, GC gc,
	const unsigned short width, const unsigned short height,
	void *image
#if !defined(USE_XPM) || !defined(USE_XBM)
	__attribute((unused))
#endif
	);

void ARWMButton_delete(ARWMButton * button);

void ARWMButton_draw(ARWMButton * button);

#endif /* ARWMBUTTON_H */
