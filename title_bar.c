// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "title_bar.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "screen.h"
#include "util.h"
#include <X11/Xatom.h>
#ifdef USE_XFT
#include "config.h"
#include <X11/Xft/Xft.h>
#endif//USE_XFT
void jbwm_toggle_shade(struct JBWMClient * restrict c)
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
		jbwm_move_resize(c);
		jbwm_set_wm_state(c, NormalState);
		jbwm_ewmh_remove_state(c->window, ewmh[WM_STATE_SHADED]);
	} else {		// Shade the client
		c->old_size.height = c->size.height;
		c->size.height = -1;
		c->opt.shaded = true;
		jbwm_set_wm_state(c, IconicState);
		jbwm_ewmh_add_state(c->window, ewmh[WM_STATE_SHADED]);
		jbwm_select_client(c);
	}
	jbwm_update_title_bar(c);
}
static uint16_t mv(const jbwm_window_t w, uint16_t x)
{
	x -= TDIM;
	XMoveWindow(jbwm.d, w, x, 0);
	return x;
}
static void move_buttons(struct JBWMClientTitlebar * restrict t,
	const uint16_t width)
{
	mv(t->stick, mv(t->shade, mv(t->resize, width)));
}
static jbwm_window_t get_win(const Window p, const jbwm_pixel_t bg)
{
	return XCreateSimpleWindow(jbwm.d, p, 0, 0, TDIM, TDIM, 0, 0, bg);
}
static jbwm_window_t new_title_bar(struct JBWMClient * restrict c)
{
	const struct JBWMPixels * p = &c->screen->pixels;
	const jbwm_window_t t = c->tb.win = get_win(c->parent, p->bg);
	XSelectInput(jbwm.d, t, ExposureMask);
	c->tb.close = get_win(t, p->close);
	c->tb.resize = get_win(t, p->resize);
	c->tb.shade = get_win(t, p->shade);
	c->tb.stick = get_win(t, p->stick);
	XMapRaised(jbwm.d, t);
	XMapSubwindows(jbwm.d, t);
	jbwm_grab_button(t, 0, AnyButton);
#ifdef JBWM_USE_EWMH
	// Required by wm-spec 1.4:
	const uint8_t b = c->border;
	jbwm_set_property(c->window, ewmh[FRAME_EXTENTS], XA_CARDINAL,
		(&(jbwm_atom_t[]){b, b, b + TDIM, b}), 4);
#endif//JBWM_USE_EWMH
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
	char * name = jbwm_get_title(c->window);
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
static void remove_title_bar(struct JBWMClient * restrict c)
{
	c->ignore_unmap++;
	XDestroyWindow(jbwm.d, c->tb.win);
	c->tb.win = 0;
#ifdef JBWM_USE_EWMH
	// Required by wm-spec 1.4:
	const uint8_t b = c->border;
	jbwm_set_property(c->window, ewmh[FRAME_EXTENTS], XA_CARDINAL,
		(&(jbwm_atom_t[]){b, b, b, b}), 4);
#endif//JBWM_USE_EWMH
}
void jbwm_update_title_bar(struct JBWMClient * c)
{
	if (c->opt.shaped)
		return;
	jbwm_window_t w = c->tb.win;
	if (c->opt.fullscreen && w) {
		remove_title_bar(c);
		return;
	}
	if (!w)
		w = new_title_bar(c);
	/* Expand/Contract the title_bar width as necessary:  */
	{
		const uint16_t width = c->size.width;
		XResizeWindow(jbwm.d, w, width, TDIM);
		move_buttons(&c->tb, width);
	}
	XClearWindow(jbwm.d, w);
	draw_title(c);
	if (c->opt.no_title_bar)
		remove_title_bar(c);
}
