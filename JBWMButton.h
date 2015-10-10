#ifndef JBWMBUTTON_H
#define JBWMBUTTON_H

typedef struct _JBWMButton
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
} JBWMButton;

JBWMButton *JBWMButton_new(Window parent, GC gc,
	const unsigned short width, const unsigned short height,
	void *image
#if !defined(USE_XPM) || !defined(USE_XBM)
	__attribute((unused))
#endif
	);

void JBWMButton_delete(JBWMButton * button);

void JBWMButton_draw(JBWMButton * button);

#endif /* JBWMBUTTON_H */
