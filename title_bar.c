// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "title_bar.h"
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include "JBWMAtomIndex.h"
#include "config.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "font.h"
#include "move_resize.h"
#include "screen.h"
#include "util.h"
#include "wm_state.h"
// This implements window shading, a substitute for iconification.
void jbwm_toggle_shade(struct JBWMClient * restrict c)
{
	// Honor !MJBWM_EWMH_WM_FUNC_MINIMIZE
	if (c->opt.no_min || c->opt.fullscreen)
		return;
	const bool s = c->opt.shaded = !c->opt.shaded;
	int8_t state = 0;
	void (*f)(Display *, Window, Atom);
	if (s) {
		c->old_size.height = c->size.height;
		c->size.height = -1;
		state = IconicState;
		f = &jbwm_ewmh_add_state;
	} else {
		c->size.height = c->old_size.height;
		state = NormalState;
		f = &jbwm_ewmh_remove_state;
	}
	jbwm_move_resize(c);
	jbwm_set_wm_state(c, state);
	f(c->display, c->window, jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_SHADED));
}
static uint16_t mv(Display * d, const Window w, uint16_t x)
{
	if (w)
		XMoveWindow(d, w, x -= jbwm_get_font_height(), 0);
	return x;
}
static void move_buttons(Display * d,
	struct JBWMClientTitlebar * t,
	const uint16_t width)
{
	mv(d, t->stick, mv(d, t->shade, mv(d, t->resize, width)));
}
static Window get_win(Display * d, const Window p,
	const jbwm_pixel_t bg)
{
	uint8_t h = jbwm_get_font_height();
	return XCreateSimpleWindow(d, p, 0, 0, h, h, 0, 0, bg);
}
static void add_buttons(struct JBWMClient * restrict c,
	const struct JBWMPixels * restrict p, const Window t)
{
	struct JBWMClientOptions * o = &c->opt;
	Display * d = c->display;
	c->tb.close = o->no_close ? 0 : get_win(d, t, p->close);
	c->tb.resize = o->no_resize ? 0: get_win(d, t, p->resize);
	c->tb.shade = o->no_min ? 0 : get_win(d, t, p->shade);
	c->tb.stick = get_win(d, t, p->stick);
}
static void configure_title_bar(Display * d, const Window t)
{
	XSelectInput(d, t, ExposureMask);
	XMapRaised(d, t);
	XMapSubwindows(d, t);
	jbwm_grab_button(d, t, None);
}
static Window new_title_bar(struct JBWMClient * restrict c)
{
	const struct JBWMPixels * p = &jbwm_get_screen(c)->pixels;
	Display * d = c->display;
	const Window t = c->tb.win = get_win(d, c->parent, p->bg);
	add_buttons(c, p, t);
	configure_title_bar(d, t);
	return t;
}
static void draw_xft(struct JBWMClient * restrict c,
	const int16_t * restrict p, char * restrict name, const size_t l)
{
	struct JBWMScreen * scr = jbwm_get_screen(c);
	XftDraw * restrict xd = scr->xft;
	XftDrawChange(xd, c->tb.win); // set target
	XftDrawStringUtf8(xd, &scr->font_color, jbwm_get_font(),
		p[0], p[1], (XftChar8 *)name, l);
}
// Free result with XFree if not NULL
static inline char * jbwm_get_title(Display * d, const Window w)
{
	return jbwm_get_property(d, w, XA_WM_NAME, &(uint16_t){0});
}
static void draw_title(struct JBWMClient * restrict c)
{
	char * name = jbwm_get_title(c->display, c->window);
	if(!name)
		return; // No title could be loaded, abort
	const int16_t p[] = {jbwm_get_font_height() + 4,
		jbwm_get_font_ascent()};
	draw_xft(c, p, name, strlen(name));
	XFree(name);
}
static void remove_title_bar(struct JBWMClient * restrict c)
{
	++(c->ignore_unmap);
	XDestroyWindow(c->display, c->tb.win);
	c->tb.win = 0;
}
void jbwm_update_title_bar(struct JBWMClient * restrict c)
{
	if (c->opt.shaped)
		return;
	Window w = c->tb.win;
	if (c->opt.fullscreen && w) {
		remove_title_bar(c);
		return;
	}
	if (!w)
		w = new_title_bar(c);
	/* Expand/Contract the title bar width as necessary:  */
	{ // * d scope
		Display * d = c->display;
		{ // width scope
			const uint16_t width = c->size.width;
			XResizeWindow(c->display, w, width,
				jbwm_get_font_height());
			move_buttons(d, &c->tb, width);
		}
		XClearWindow(d, w);
	}
	draw_title(c);
	if (c->opt.no_title_bar)
		remove_title_bar(c);
}
