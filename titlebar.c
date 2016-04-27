// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "titlebar.h"

#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "log.h"
#include "screen.h"
#include "util.h"

#include <assert.h>
#include <string.h>
#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif//USE_XFT

void shade(Client * restrict c)
{
	LOG("shade");
	assert(c);

	// Honor !MWM_FUNC_MINIMIZE
	if (c->flags & JB_NO_MIN)
		return;

	/* This implements window shading, a substitute
	   for iconification.  */
	if (c->flags & JB_SHADED) {
		// Unshade
		//c->size.height = c->shade_height;
		c->size.height = c->old_size.height;
		c->flags &= ~JB_SHADED;
		XMapWindow(jbwm.dpy, c->window);
		moveresize(c);
		set_wm_state(c, NormalState);
		ewmh_remove_state(c->window, ewmh.atoms[WM_STATE_SHADED]);
	} else {		// Shade the client
		//c->shade_height = c->size.height;
		c->old_size.height = c->size.height;
		c->ignore_unmap++;
		XUnmapWindow(jbwm.dpy, c->window);
		c->size.height = 0;
		c->flags |= JB_SHADED;
		set_wm_state(c, IconicState);
		ewmh_add_state(c->window, ewmh.atoms[WM_STATE_SHADED]);
		select_client(c);
	}
}

static GC colorgc(ScreenInfo * restrict s, const char *restrict colorname)
{
	return XCreateGC(jbwm.dpy, s->root, GCForeground,
		&(XGCValues){.foreground=pixel(s->screen, colorname)});
}

static void setup_gcs(ScreenInfo * restrict s)
{
	jbwm.gc.close = colorgc(s, TITLEBAR_CLOSE_BG);
	jbwm.gc.shade = colorgc(s, TITLEBAR_SHADE_BG);
	jbwm.gc.resize = colorgc(s, TITLEBAR_RESIZE_BG);
}

static void new_titlebar(Client * restrict c)
{
	if (c->flags & (JB_NO_TB|JB_SHAPED))
		return;

	c->titlebar = XCreateSimpleWindow(jbwm.dpy, c->parent, 0, 0,
		c->size.width, TDIM, 0, 0, c->screen->pixels.bg);
	if (!jbwm.gc.close)
		setup_gcs(c->screen);

	XSelectInput(jbwm.dpy, c->titlebar, ExposureMask);
	XMapRaised(jbwm.dpy, c->titlebar);
	jbwm_grab_button(c->titlebar, 0, AnyButton);
}

#ifdef USE_XFT
static void
draw_xft(Client * restrict c, const XPoint * restrict p,
	 char *restrict name, const size_t l)
{
	XGlyphInfo e;
	XftTextExtentsUtf8(jbwm.dpy, jbwm.font, (XftChar8 *) name, l, &e);
	const uint8_t s = c->screen->screen;
	Visual *v = DefaultVisual(jbwm.dpy, s);
	const Colormap cm = DefaultColormap(jbwm.dpy, s);
	XftDraw *xd = XftDrawCreate(jbwm.dpy, c->titlebar, v, cm);
	XftColor color;
	XftColorAllocName(jbwm.dpy, v, cm, DEF_fg, &color);
	/* Prevent the text from going over the resize button.  */
	const unsigned short max_width = c->size.width - 3 * TDIM;
	XftDrawStringUtf8(xd, &color, jbwm.font, p->x, p->y,
			  (XftChar8 *) name,
			  e.width > max_width
			  && e.width > 0 ? l * max_width / e.width : l);
	XftDrawDestroy(xd);
	XftColorFree(jbwm.dpy, v, cm, &color);
}
#endif//USE_XFT

GC tbgc;

static void draw_title(Client * restrict c)
{
	char * name = get_title(c->window);
	if(!name) return; // No title could be loaded, abort
	const XPoint p = { TDIM + 4, jbwm.font->ascent - JBWM_BORDER };
#ifdef USE_XFT
	draw_xft(c, &p, name, strlen(name));
#else//!USE_XFT
	XDrawString(jbwm.dpy, c->titlebar, c->screen->gc, p.x, p.y,
		name, strlen(name));
#endif//USE_XFT
	XFree(name);
}

static void draw(const Window t, GC gc, const int x)
{
	XFillRectangle(jbwm.dpy, t, gc, x, 0, TDIM, TDIM);
}

static void draw_titlebar(Client * restrict c)
{
	const unsigned short w = c->size.width;
	const Window t = c->titlebar;
	XClearWindow(jbwm.dpy, t);
	const uint32_t f = c->flags;
	if (!(f & JB_NO_CLOSE_DECOR))
		draw(t, jbwm.gc.close, 0);
	if (!(f & JB_NO_MIN_DECOR))
		draw(t, jbwm.gc.shade, w-TDIM);
	if (!(f & JB_NO_MIN_DECOR))
		draw(t, jbwm.gc.resize, w-(TDIM<<1));
	if(f & JB_TEAROFF)
		  return;
	draw_title(c);
}

void update_titlebar(Client * c)
{
	assert(c);

	if (c->flags & (JB_NO_TB | JB_SHAPED))
		return;

	if (c->flags & JB_FULLSCREEN) {
		/* May generate BadWindow on subsequent invocations,
		   however the error handler makes such irrelevant.  */
		XDestroyWindow(jbwm.dpy, c->titlebar);
		c->titlebar = 0;
		return;
	}

	if (!c->titlebar) {
		new_titlebar(c);
		/* Return here to prevent BadWindow/BadDrawable errors */
		return;
	}

	/* Expand/Contract the titlebar width as necessary:  */
	XMoveResizeWindow(jbwm.dpy, c->titlebar, 0, 0, c->size.width, TDIM);
	draw_titlebar(c);
}
