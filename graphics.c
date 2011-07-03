/* Copyright 2008, Jeffrey E. Bedard <antiright@gmail.com> */

#include "arwm.h"

unsigned long
arwm_get_pixel_for_color(const int r, const int g, const int b)
{
	XColor color;
	Display * dpy = arwm.X.dpy;
	const int screen = arwm.X.screens ? arwm.X.screens->screen
		: DefaultScreen(dpy);

	color.red=r*255; 
	color.green=g*255;
	color.blue=b*255;

	XAllocColor(dpy, DefaultColormap(dpy, screen), &color);

	return color.pixel;
}

inline ARWMColor
get_ARWMColor_for(int r, int g, int b)
{
	ARWMColor color;

	color.r=r;
	color.g=g;
	color.b=b;

	return color;
}

inline GC
arwm_new_gc_for_ARWMColor_fg(ARWMColor fg)
{
	return arwm_new_gc_for_ARWMColor(fg, get_ARWMColor_for(0, 0, 0));
}

inline GC
arwm_new_gc_for_ARWMColor(ARWMColor fg, ARWMColor bg)
{
	return arwm_new_gc_for_color(fg.r, fg.g, fg.b,
			bg.r, bg.g, bg.b);
}

GC
arwm_new_gc_for_color(	int fg_r, int fg_g, int fg_b, 
	int bg_r, int bg_g, int bg_b)
{
	unsigned long valuemask;
	XGCValues values;
	Display * dpy = arwm.X.dpy;
	GC gc;

	values.foreground=arwm_get_pixel_for_color(fg_r,fg_g,fg_b);
	values.background=arwm_get_pixel_for_color(bg_r,bg_g,bg_b);
	valuemask = GCForeground | GCBackground;
	gc=XCreateGC(dpy, DefaultRootWindow(dpy), 
		valuemask, &values);

	return gc;	
}


static void
delete_ARWMButton(ARWMButton * button)
{
	if(button->flags & __ARWMBUTTON_ALLOCATED_GEOMETRY)
		free(button->geometry);
	if(button->flags & __ARWMBUTTON_ALLOCATED_GC)
		XFreeGC(arwm.X.dpy, button->gc);
	free(button);
}

static void
draw_background(const XRectangle * geo)
{
	XFillRectangle(arwm.X.dpy, 
			arwm.titlebar.buttons.handle->parent,
			arwm.titlebar.buttons.handle->gc,
			geo->x, geo->y, geo->width, geo->height);
}

static void
draw_highlight(const XRectangle modgeo, const int active)
{
	GC gc;			

	gc = arwm_new_gc_for_ARWMColor_fg(
		get_ARWMColor_for(
			0xae/(active?1:2), 
			0xb2/(active?1:2), 
			0xc3/(active?1:2)));
	XFillArc(arwm.X.dpy, arwm.titlebar.buttons.handle->parent, gc, 
		modgeo.x+3, modgeo.y+3,
		modgeo.width-6, modgeo.height-6, 
		0, 23040/2);
		
	XFreeGC(arwm.X.dpy, gc);
}

static void
draw_gradient(const XRectangle * geo, const XRectangle modgeo, 
	const ARWMColor color)
{
	int i;

	for(i=0; i < geo->height; i+=1)
	{
		GC gc;
		{
			const int j = 16*i;

			gc = arwm_new_gc_for_ARWMColor_fg(
				get_ARWMColor_for(
					color.r+j, color.g+j, color.b+j));
		}
		{
			const int k = 2*i;

			XFillArc(arwm.X.dpy, 
				arwm.titlebar.buttons.handle->parent, gc, 
				modgeo.x+i, modgeo.y+i,
				modgeo.width-k, modgeo.height-k, 
				0, 23040);
		}
		XFreeGC(arwm.X.dpy, gc);
	}
}

static void
draw_shadow(const XRectangle * geo)
{
	GC gc;

	gc = arwm_new_gc_for_ARWMColor_fg(
		get_ARWMColor_for(0x40,0x40,0x50));
	XFillArc(arwm.X.dpy, arwm.titlebar.buttons.handle->parent, gc, 
		geo->x+1, geo->y+1, geo->width-2, geo->height-2, 11520, 11520);
	XFreeGC(arwm.X.dpy, gc);
}

static ARWMColor
get_button_color(const ARWMColor color, const int active)
{
	ARWMColor result;

	if(active)
	{
		result.r=color.r/1.5;
		result.b=color.b/1.5; 
		result.g=color.g/1.5;
	}
	else
	{
		result.r=result.b=result.g=0x30;
	}

	return result;
}

static void
draw_button_graphic(ARWMButton * button)
{
	const XRectangle * geo = button->geometry;

	draw_background(geo);
	if(button->flags & ARWMBUTTON_IS_CONTAINER)
		return;
	draw_shadow(geo);
	{
		XRectangle modgeo;
		const int active = (button->c->flags & AR_CLIENT_ACTIVE)
			? True : False;

		modgeo.x=geo->x+2;
		modgeo.y=geo->y+2;
		modgeo.width=geo->width-4;
		modgeo.height=geo->height-4;

		draw_gradient(geo, modgeo, 
			get_button_color(button->color, active));
		draw_highlight(modgeo, active);
	}
}

static void
ARWMButton_set_color(ARWMButton * button, ARWMColor color)
{
	button->color=color;
	if(button->flags & __ARWMBUTTON_ALLOCATED_GC)
		XFreeGC(arwm.X.dpy, button->gc);
	button->gc=arwm_new_gc_for_ARWMColor_fg(color);
}

int
arwm_get_text_height()
{
	XGlyphInfo extents;
	const char * s = "FOO";
	
	XftTextExtentsUtf8(arwm.X.dpy, arwm.X.font, 
			(XftChar8*)s, 4, &extents);

	return extents.height;
}

int
arwm_draw_string(Window target, 
		XRectangle * geometry, 
		const char * colorname,
		const char * string)
{
	XftDraw * draw;
	XftColor color;
	XGlyphInfo extents;
	int length;
	Display * dpy = arwm.X.dpy;
	const int scr=DefaultScreen(dpy);
	Visual *vis=DefaultVisual(dpy, scr);
	const Colormap cmap=DefaultColormap(dpy, scr);
	
	if(!string)
		return -1;
	XftColorAllocName(dpy, vis, cmap, colorname, &color);
	draw=XftDrawCreate(dpy, target, vis, cmap);
	length = strlen(string);
	XftDrawChange(draw, target);
	XftTextExtentsUtf8(arwm.X.dpy, arwm.X.font, (XftChar8*)string,
		length, &extents);
	if(extents.width > geometry->width)
		length=length*(geometry->width)/(extents.width);
	XftDrawStringUtf8(draw, &color, arwm.X.font, 
			geometry->x + 4,
			geometry->y+extents.height+(extents.height>=13 ? 0 : 3),
			(XftChar8*)string, length);
	XftColorFree(dpy, XftDrawVisual(draw), 
		XftDrawColormap(draw), &color);
	XftDrawDestroy(draw);

	return length;
}

static void
ARWMButton_draw(ARWMButton * button)
{
	draw_button_graphic(button);
	if(button->text)
	{
		arwm_draw_string(button->parent, button->geometry, 
			ARWM_TITLEBAR_FOREGROUND, button->text);
	}
}

ARWMButton *
new_ARWMButton(Window parent)
{
	ARWMButton * button;

	button=malloc(sizeof(ARWMButton));
	button->delete=&delete_ARWMButton;
	button->draw=&ARWMButton_draw;
	button->set_color=&ARWMButton_set_color;
	button->flags=None;
	button->text=NULL;
	button->parent=parent;

	button->gc=arwm_new_gc_for_color(0xae,0xb2,0xc3, 0,0,0);
	button->flags |= __ARWMBUTTON_ALLOCATED_GC;
	button->geometry=calloc(1, sizeof(XRectangle));
	button->flags |= __ARWMBUTTON_ALLOCATED_GEOMETRY;

	return button;
}

