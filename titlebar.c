// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "titlebar.h"

#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "screen.h"
#include "util.h"

#include <stdlib.h>
#include <X11/Xatom.h>

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif//USE_XFT

void shade(struct JBWMClient * restrict c)
{
	// Honor !MWM_FUNC_MINIMIZE
	if (c->opt.no_min || c->opt.fullscreen)
		return;

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

static void move_buttons(struct JBWMClient * restrict c)
{
	uint16_t x = c->size.width - TDIM;
	XMoveWindow(jbwm.d, c->tb.resize, x, 0);
	x -= TDIM;
	XMoveWindow(jbwm.d, c->tb.shade, x, 0);
}

static Window get_win(const Window p, const uint16_t w, const uint16_t h,
	const jbwm_pixel_t bg)
{
	return XCreateSimpleWindow(jbwm.d, p, 0, 0, w, h, 0, 0, bg);
}

static Window get_button(const Window p, const jbwm_pixel_t bg)
{
	return get_win(p, TDIM, TDIM, bg);
}

static jbwm_window_t new_titlebar(struct JBWMClient * restrict c)
{
	const struct JBWMPixels * p = &c->screen->pixels;
	const jbwm_window_t t = c->tb.win = get_win(c->parent,
		c->size.width, TDIM, p->bg);
	XSelectInput(jbwm.d, t, ExposureMask);
	c->tb.close = get_button(t, p->close);
	c->tb.resize = get_button(t, p->resize);
	c->tb.shade = get_button(t, p->shade);
	move_buttons(c);
	XMapRaised(jbwm.d, t);
	XMapSubwindows(jbwm.d, t);
	jbwm_grab_button(t, 0, AnyButton);
#ifdef EWMH
	// Required by wm-spec 1.4:
	const uint8_t b = c->border;
	XPROP(c->window, ewmh[FRAME_EXTENTS], XA_CARDINAL,
		(&(jbwm_atom_t[]){b, b, b + TDIM, b}), 4);
#endif//EWMH
	return t;
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
	XftDraw *xd = XftDrawCreate(jbwm.d, c->tb.win, v, cm);
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
	XDrawString(jbwm.d, c->tb.win, c->screen->gc,
		p.x, p.y, name, l);
#endif//USE_XFT
	XFree(name);
}

static void remove_titlebar(struct JBWMClient * restrict c)
{
	c->ignore_unmap++;
	XDestroyWindow(jbwm.d, c->tb.win);
	c->tb.win = 0;
#ifdef EWMH
	// Required by wm-spec 1.4:
	const uint8_t b = c->border;
	XPROP(c->window, ewmh[FRAME_EXTENTS], XA_CARDINAL,
		(&(jbwm_atom_t[]){b, b, b, b}), 4);
#endif//EWMH
}

void update_titlebar(struct JBWMClient * c)
{
	if (c->opt.no_titlebar || c->opt.shaped)
		  return;
	jbwm_window_t w = c->tb.win;
	if (c->opt.fullscreen && w) {
		remove_titlebar(c);
		return;
	}

	if (!w)
		w = new_titlebar(c);

	/* Expand/Contract the titlebar width as necessary:  */
	XResizeWindow(jbwm.d, w, c->size.width, TDIM);
	move_buttons(c);
	XClearWindow(jbwm.d, w);
	draw_title(c);
}

