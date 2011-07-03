#ifndef ARWM_GRAPHICS_H
#define ARWM_GRAPHICS_H

typedef struct _ARWMColor
{
	int r, g, b;
} ARWMColor;

typedef struct _ARWMButton
{
	Client * c;
	Window parent;
	GC gc;
	XRectangle * geometry;
	ARWMColor color;
	char * text;

	unsigned int flags;
#define __ARWMBUTTON_ALLOCATED_GC (1<<0)
#define __ARWMBUTTON_ALLOCATED_GEOMETRY (1<<1)
#define ARWMBUTTON_IS_CONTAINER (1<<2)

	void (*draw)(struct _ARWMButton *);
	void (*delete)(struct _ARWMButton *);
	void (*set_color)(struct _ARWMButton *, ARWMColor);

} ARWMButton;


ARWMButton *
new_ARWMButton(Window parent);

unsigned long
arwm_get_pixel_for_color(const int r, const int g, const int b);

GC
arwm_new_gc_for_color(int fg_r, int fg_g, int fg_b, 
	int bg_r, int bg_g, int bg_b);

GC
arwm_new_gc_for_ARWMColor(ARWMColor fg, ARWMColor bg);

GC
arwm_new_gc_for_ARWMColor_fg(ARWMColor fg);

ARWMColor
get_ARWMColor_for(int r, int g, int b);

XImage *
arwm_get_XImage_for_XBM(unsigned char * data, 
	unsigned int width, unsigned int height);


int
arwm_draw_string(Window target, 
		XRectangle * geometry, 
		const char * colorname,
		const char * string);

#endif /* ARWM_GRAPHICS_H */

