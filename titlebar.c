// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "titlebar.h"

#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "log.h"
#include "screen.h"
#include "util.h"

#include <stdlib.h>

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif//USE_XFT

void shade(struct JBWMClient * restrict c)
{
	LOG("shade");

	// Honor !MWM_FUNC_MINIMIZE
	if (c->opt.no_min || c->opt.fullscreen) return;

	/* This implements window shading, a substitute
	   for iconification.  */
	if (c->opt.shaded) {
		// Unshade
		c->size.height = c->old_size.height;
		c->opt.shaded = false;
		XMapWindow(jbwm.d, c->window);
		moveresize(c);
		set_wm_state(c, NormalState);
		ewmh_remove_state(c->window, ewmh[WM_STATE_SHADED]);
	} else {		// Shade the client
		c->old_size.height = c->size.height;
		c->ignore_unmap++;
		XUnmapWindow(jbwm.d, c->window);
		c->size.height = 0;
		c->opt.shaded = true;
		set_wm_state(c, IconicState);
		ewmh_add_state(c->window, ewmh[WM_STATE_SHADED]);
		select_client(c);
	}
	update_titlebar(c);
}

static GC colorgc(struct JBWMScreen * restrict s, const char *restrict colorname)
{
	return XCreateGC(jbwm.d, s->root, GCForeground,
		&(XGCValues){.foreground=pixel(s->screen, colorname)});
}

static void setup_gcs(struct JBWMScreen * restrict s)
{
	jbwm.gc.close = colorgc(s, getenv(JBWM_ENV_CLOSE));
	jbwm.gc.shade = colorgc(s, getenv(JBWM_ENV_SHADE));
	jbwm.gc.resize = colorgc(s, getenv(JBWM_ENV_RESIZE));
}

static void new_titlebar(struct JBWMClient * restrict c)
{
	if (c->opt.no_titlebar || c->opt.shaped)
		return;

	c->titlebar = XCreateSimpleWindow(jbwm.d, c->parent, 0, 0,
		c->size.width, TDIM, 0, 0, c->screen->pixels.bg);
	if (!jbwm.gc.close)
		setup_gcs(c->screen);

	XSelectInput(jbwm.d, c->titlebar, ExposureMask);
	XMapRaised(jbwm.d, c->titlebar);
	jbwm_grab_button(c->titlebar, 0, AnyButton);
}

#ifdef USE_XFT
static void
draw_xft(struct JBWMClient * restrict c, const XPoint * restrict p,
	 char * restrict name, const size_t l)
{
	XGlyphInfo e;
	XftTextExtentsUtf8(jbwm.d, jbwm.font, (XftChar8 *) name, l, &e);
	const uint8_t s = c->screen->screen;
	Visual *v = DefaultVisual(jbwm.d, s);
	const Colormap cm = DefaultColormap(jbwm.d, s);
	XftDraw *xd = XftDrawCreate(jbwm.d, c->titlebar, v, cm);
	XftColor color;
	XftColorAllocName(jbwm.d, v, cm, getenv(JBWM_ENV_FG), &color);
	/* Prevent the text from going over the resize button.  */
	const uint16_t max_width = c->size.width - 3 * TDIM;
	XftDrawStringUtf8(xd, &color, jbwm.font, p->x, p->y,
			  (XftChar8 *) name,
			  e.width > max_width
			  && e.width > 0 ? l * max_width / e.width : l);
	XftDrawDestroy(xd);
	XftColorFree(jbwm.d, v, cm, &color);
}
#endif//USE_XFT

static void draw_title(struct JBWMClient * restrict c)
{
	char * name = get_title(c->window);
	if(!name) return; // No title could be loaded, abort
	const XPoint p = { TDIM + 4, jbwm.font->ascent };
	size_t l = 0; // strlen
	while(name[++l])
		  ;
#ifdef USE_XFT
	draw_xft(c, &p, name, l);
#else//!USE_XFT
	XDrawString(jbwm.d, c->titlebar, c->screen->gc,
		p.x, p.y, name, l);
#endif//USE_XFT
	XFree(name);
}

static inline void draw(const Window t, GC gc, const uint16_t x)
{
	XFillRectangle(jbwm.d, t, gc, x, 0, TDIM, TDIM);
}

static void draw_titlebar(struct JBWMClient * restrict c)
{
	const Window t = c->titlebar;
	XClearWindow(jbwm.d, t);
	if (!c->opt.no_close_decor)
		draw(t, jbwm.gc.close, 0);
	if (c->opt.tearoff) return;
	const uint16_t w = c->size.width-TDIM;
	if (!c->opt.no_resize_decor && !c->opt.shaded)
		draw(t, jbwm.gc.resize, w);
	if (!c->opt.no_min_decor)
		draw(t, jbwm.gc.shade, w-TDIM);
	draw_title(c);
}

static void remove_titlebar(struct JBWMClient * restrict c)
{
	c->ignore_unmap++;
	XDestroyWindow(jbwm.d, c->titlebar);
	c->titlebar = 0;
}

void update_titlebar(struct JBWMClient * c)
{
	if (c->opt.no_titlebar || c->opt.shaped)
		  return;

	if (c->opt.fullscreen && c->titlebar) {
		remove_titlebar(c);
		return;
	}

	if (!c->titlebar) {
		new_titlebar(c);
		/* Return here to prevent BadWindow/BadDrawable errors */
		return;
	}

	/* Expand/Contract the titlebar width as necessary:  */
	XMoveResizeWindow(jbwm.d, c->titlebar, 0, 0, c->size.width, TDIM);
	draw_titlebar(c);
}

