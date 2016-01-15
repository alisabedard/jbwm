// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "jbwm.h"

static void
setup_gcs(const Window w)
{
	XColor c=jbwm_color(TITLEBAR_CLOSE_BG);
	XGCValues v;
	v.foreground=c.pixel;
	jbwm.gc.close=XCreateGC(D, w, GCForeground, &v);
	c=jbwm_color(TITLEBAR_SHADE_BG);
	v.foreground=c.pixel;
	jbwm.gc.shade=XCreateGC(D, w, GCForeground, &v);
	c=jbwm_color(TITLEBAR_RESIZE_BG);
	v.foreground=c.pixel;
	jbwm.gc.resize=XCreateGC(D, w, GCForeground, &v);
	jbwm.gc.tb_initialized=true;
#ifndef USE_XFT
	v.font=jbwm.X.font->fid;
	v.function=GXinvert;
	jbwm.gc.handle=XCreateGC(D, w, GCFont|GCFunction, &v);
#endif//USE_XFT
}

static void
new_titlebar(Client *c)
{
	if(c->flags&JB_NO_TB)
		return;
#ifdef USE_SHAPE
	if(c->flags&JB_SHAPED)
		return;
#endif /* USE_SHAPE */
	const Window w = c->titlebar = XCreateSimpleWindow(D, c->parent, 0, 0, 
		c->size.width, TDIM, 0, 0, 0); 
	if(!jbwm.gc.tb_initialized)
		setup_gcs(w);
	XSelectInput(D, w, ExposureMask);
	XSetWindowBackground(D, w, c->screen->bg.pixel);
	XMapRaised(D, w);
	jbwm_grab_button(w, 0, AnyButton);
}

#ifdef USE_XFT
static void
draw_xft(Client *c, const Position *p, char *name, const size_t l)
{
	XGlyphInfo e;
	XftTextExtentsUtf8(D, jbwm.X.font, (XftChar8 *) name, l, &e);
	const uint8_t s=c->screen->screen;
	Visual *v=DefaultVisual(D, s);
	const Colormap cm=DefaultColormap(D, s);
	XftDraw *xd=XftDrawCreate(D, c->titlebar, v, cm);
	XftColor color;
	XftColorAllocName(D, v, cm, DEF_FG, &color);
	/* Prevent the text from going over the resize button.  */
	const unsigned short max_width = c->size.width - 3 * TDIM;
	XftDrawStringUtf8(xd, &color, jbwm.X.font, p->x, p->y, 
		(XftChar8 *) name, e.width > max_width && e.width > 0 
		? l * max_width / e.width : l);
	XftDrawDestroy(xd);
	XftColorFree(D, v, cm, &color);
}
#endif//USE_XFT

GC tbgc;

static void
draw_title(Client * c)
{
	XTextProperty tp;
	if(!XGetWMName(D, c->window, &tp))
		return;
	char *name=(char*)tp.value;
	const size_t l = strlen(name);
	const Position p = {TDIM+4, jbwm.X.font->ascent-JBWM_BORDER};
#ifdef USE_XFT
	draw_xft(c, &p, name, l);
#else//!USE_XFT
	XDrawString(D, c->titlebar, jbwm.gc.handle, p.x, p.y, name, l);
#endif//USE_XFT
	XFree(name);
}

static void
draw(const Window t, GC gc, const int x)
{
	XFillRectangle(D, t, gc, x, 0, TDIM, TDIM);
}

static inline void
draw_close(const uint32_t f, const Window t)
{
	if(!(f&JB_NO_CLOSE_DECOR))
		draw(t, jbwm.gc.close, 0);
}

static inline void
draw_shade(const uint32_t f, const Window t, const int x)
{
	if(!(f&JB_NO_MIN_DECOR))
		draw(t, jbwm.gc.shade, x);
}

static inline void
draw_resize(const uint32_t f, const Window t, const int x)
{
	if(!(f&JB_NO_MIN_DECOR))
		draw(t, jbwm.gc.resize, x);
}

static inline int
tboffset(const int w, const int n)
{
	return w-n*TDIM;
}

static void
draw_titlebar(Client * c)
{
	const unsigned short w = c->size.width;
	const Window t=c->titlebar;
	XClearWindow(D, t);
	const uint32_t f=c->flags;
	draw_close(f, t);
	draw_resize(f, t, tboffset(w, 1));
	draw_shade(f, t, tboffset(w, 2));
	if(!(c->flags&JB_TEAROFF))
		draw_title(c);
}

void
update_titlebar(Client * c)
{
	assert(c);
	if(c->flags & JB_NO_TB)
		return;
#ifdef USE_SHAPE
	if(c->flags & JB_SHAPED)
		return;
#endif//USE_SHAPE
	if(c->flags & JB_MAXIMIZED)
	{
		/* May generate BadWindow on subsequent invocations,
		   however the error handler makes such irrelevant.  */
		XDestroyWindow(D, c->titlebar);
		c->titlebar=0;
		return;
	}
	if(!c->titlebar)
	{
		new_titlebar(c);
		/* Return here to prevent BadWindow/BadDrawable errors */
		return;
	}

	/* Expand/Contract the titlebar width as necessary:  */
	XMoveResizeWindow(D, c->titlebar, 0, 0, c->size.width, TDIM);
	draw_titlebar(c);
}

