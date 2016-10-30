// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "title_bar.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "font.h"
#include "jbwm.h"
#include "screen.h"
#include "util.h"
#include <X11/Xatom.h>
#ifdef JBWM_USE_XFT
#include "config.h"
#include <X11/Xft/Xft.h>
#endif//JBWM_USE_XFT
static void set_shaded(Display * restrict d,
	struct JBWMClient * restrict c)
{
	c->old_size.height = c->size.height;
	c->size.height = -1;
	c->opt.shaded = true;
	jbwm_set_wm_state(d, c, IconicState);
	jbwm_ewmh_add_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_SHADED));
	jbwm_select_client(d, c);
}
static void set_not_shaded(Display * restrict d,
	struct JBWMClient * restrict c)
{
	c->size.height = c->old_size.height;
	c->opt.shaded = false;
	jbwm_move_resize(d, c);
	jbwm_set_wm_state(d, c, NormalState);
	jbwm_ewmh_remove_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_SHADED));
}
void jbwm_toggle_shade(Display * restrict d, struct JBWMClient * restrict c)
{
	// Honor !MJBWM_EWMH_WM_FUNC_MINIMIZE
	if (c->opt.no_min || c->opt.fullscreen)
		return;
	// This implements window shading, a substitute for iconification.
	(c->opt.shaded ? set_not_shaded : set_shaded)(d, c);
	jbwm_update_title_bar(d, c);
}
static uint16_t mv(Display * restrict d, const jbwm_window_t w, uint16_t x)
{
	if (w) {
		x -= jbwm_get_font_height();
		XMoveWindow(d, w, x, 0);
	}
	return x;
}
static void move_buttons(Display * restrict d,
	struct JBWMClientTitlebar * restrict t,
	const uint16_t width)
{
	mv(d, t->stick, mv(d, t->shade, mv(d, t->resize, width)));
}
static jbwm_window_t get_win(Display * restrict d, const Window p,
	const jbwm_pixel_t bg)
{
	uint8_t h = jbwm_get_font_height();
	return XCreateSimpleWindow(d, p, 0, 0, h, h, 0, 0, bg);
}
static jbwm_window_t new_title_bar(Display * restrict d,
	struct JBWMClient * restrict c)
{
	const struct JBWMPixels * p = &jbwm_get_screens()[c->screen].pixels;
	const jbwm_window_t t = c->tb.win = get_win(d, c->parent, p->bg);
	XSelectInput(d, t, ExposureMask);
	struct JBWMClientOptions * o = &c->opt;
	c->tb.close = o->no_close ? 0 : get_win(d, t, p->close);
	c->tb.resize = o->no_resize ? 0: get_win(d, t, p->resize);
	c->tb.shade = o->no_min ? 0 : get_win(d, t, p->shade);
	c->tb.stick = get_win(d, t, p->stick);
	XMapRaised(d, t);
	XMapSubwindows(d, t);
	jbwm_grab_button(d, t, 0, AnyButton);
#ifdef JBWM_USE_EWMH
	// Required by wm-spec 1.4:
	const uint8_t b = c->border;
	jbwm_set_property(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_FRAME_EXTENTS),
		XA_CARDINAL, (&(jbwm_atom_t[]){b, b, b
		+ jbwm_get_font_height(), b}), 4);
#endif//JBWM_USE_EWMH
	return t;
}
#ifdef JBWM_USE_XFT
static void
draw_xft(Display * restrict d, struct JBWMClient * restrict c,
	const XPoint * restrict p, char * restrict name, const size_t l)
{
	XftFont * f = jbwm_get_font();
	XGlyphInfo e;
	XftTextExtentsUtf8(d, f, (XftChar8 *) name, l, &e);
	const uint8_t s = c->screen;
	Visual *v = DefaultVisual(d, s);
	XftDraw *xd = XftDrawCreate(d, c->tb.win, v, c->cmap);
	XftColor color;
	XftColorAllocName(d, v, c->cmap, getenv(JBWM_ENV_FG), &color);
	/* Prevent the text from going over the resize button.  */
	const uint16_t max_width = c->size.width
		- 3 * jbwm_get_font_height();
	XftDrawStringUtf8(xd, &color, f, p->x, p->y,
		(XftChar8 *) name, e.width > max_width
		&& e.width > 0 ? l * max_width / e.width : l);
	XftDrawDestroy(xd);
	XftColorFree(d, v, c->cmap, &color);
}
#endif//JBWM_USE_XFT
// Free result with XFree if not NULL
__attribute__((pure))
static char * jbwm_get_title(Display * restrict d, const jbwm_window_t w)
{
	XTextProperty tp;
	if (!XGetWMName(d, w, &tp))
		  return NULL;
	return (char *)tp.value;
}
static void draw_title(Display * restrict d, struct JBWMClient * restrict c)
{
	char * name = jbwm_get_title(d, c->window);
	if(!name)
		return; // No title could be loaded, abort
	const XPoint p = { jbwm_get_font_height() + 4,
		jbwm_get_font_ascent()};
	size_t l = 0; // strlen
	while(name[++l])
		  ;
#ifdef JBWM_USE_XFT
	draw_xft(d, c, &p, name, l);
#else//!JBWM_USE_XFT
	XDrawString(d, c->tb.win, jbwm_get_screens()[c->screen].gc,
		p.x, p.y, name, l);
#endif//JBWM_USE_XFT
	XFree(name);
}
static void remove_title_bar(Display * restrict d,
	struct JBWMClient * restrict c)
{
	c->ignore_unmap++;
	XDestroyWindow(d, c->tb.win);
	c->tb.win = 0;
#ifdef JBWM_USE_EWMH
	// Required by wm-spec 1.4:
	const uint8_t b = c->border;
	jbwm_set_property(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_FRAME_EXTENTS),
		XA_CARDINAL, (&(jbwm_atom_t[]){b, b, b, b}), 4);
#endif//JBWM_USE_EWMH
}
void jbwm_update_title_bar(Display * restrict d, struct JBWMClient * c)
{
	if (c->opt.shaped)
		return;
	jbwm_window_t w = c->tb.win;
	if (c->opt.fullscreen && w) {
		remove_title_bar(d, c);
		return;
	}
	if (!w)
		w = new_title_bar(d, c);
	/* Expand/Contract the title bar width as necessary:  */
	{
		const uint16_t width = c->size.width;
		XResizeWindow(d, w, width, jbwm_get_font_height());
		move_buttons(d, &c->tb, width);
	}
	XClearWindow(d, w);
	draw_title(d, c);
	if (c->opt.no_title_bar)
		remove_title_bar(d, c);
}
