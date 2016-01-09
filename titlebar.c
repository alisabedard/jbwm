// jbwm - Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com>
#include "jbwm.h"
#include <string.h>

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif /* USE_XFT */

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
	XSelectInput(D, w, ExposureMask);
	XColor color=jbwm_color(TITLEBAR_BG);
	XSetWindowBackground(D, w, color.pixel);
	free_color(&color);
	XMapRaised(D, w);
	jbwm_grab_button(w, 0, AnyButton);
}

#ifdef USE_XFT
static void
draw_xft(Client *c, const Position *p, char *name, const size_t l)
{
	XGlyphInfo e;
	XftTextExtentsUtf8(D, jbwm.X.font, (XftChar8 *) name, l, &e);
	const ubyte s=c->screen->screen;
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

static void
draw_title(Client * c, char *name)
{
	if(!name) // Nothing to display
		return;
	const size_t l = strlen(name);
	const Position p = {.x=TDIM+8, .y=jbwm.X.font->ascent-JBWM_BORDER};
#ifdef USE_XFT
	draw_xft(c, &p, name, l);
#else//!USE_XFT
	const Window w = c->titlebar;
	GC gc=jbwm_new_gc(&c->screen->fg);
	XGCValues v={.font=jbwm.X.font->fid};
	XChangeGC(D, gc, GCFont, &v);
	XDrawString(D, w, gc, p.x, p.y, name, l);
	XFreeGC(D, gc);
#endif//USE_XFT
	XFree(name);
}

static void
draw(const Window t, const int x, const char *color)
{
	XRectangle g={.x=x, .y=0, .width=TDIM, .height=TDIM};
	draw_rectangle(t, &g, color);
}

static inline void
draw_close(const uint32_t f, const Window t)
{
	if(!(f&JB_NO_CLOSE_DECOR))
		draw(t, 0, TITLEBAR_CLOSE_BG);
}

static inline void
draw_shade(const uint32_t f, const Window t, const int x)
{
	if(!(f&JB_NO_MIN_DECOR))
		draw(t, x, TITLEBAR_SHADE_BG);
}

static inline void
draw_resize(const uint32_t f, const Window t, const int x)
{
	if(!(f&JB_NO_MIN_DECOR))
		draw(t, x, TITLEBAR_RESIZE_BG);
}

static inline int
tboffset(const int w, const int n)
{
	return w-n*TDIM;
}

static void
draw_titlebar(Client * c, char *name)
{
	const unsigned short w = c->size.width;
	const Window t=c->titlebar;
	XClearWindow(D, t);
	const uint32_t f=c->flags;
	draw_close(f, t);
	draw_resize(f, t, tboffset(w, 1));
	draw_shade(f, t, tboffset(w, 2));
	if(!(c->flags&JB_TEAROFF))
		draw_title(c, name);
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
	XTextProperty p;
	XGetTextProperty(D, c->window, &p, XA_WM_NAME);
	draw_titlebar(c, (char *)p.value);
}

