// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "screen.h"
#include <stdlib.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include "client.h"
#include "config.h"
#include "ewmh_state.h"
#include "ewmh.h"
#include "font.h"
#include "JBDim.h"
#include "jbwm.h"
#include "shape.h"
#include "snap.h"
#include "title_bar.h"
#include "util.h"
#include "wm_state.h"
//#define DEBUG_SCREEN
#ifndef DEBUG_SCREEN
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!DEBUG_SCREEN
enum {JBWMMouseMask=(ButtonPressMask|ButtonReleaseMask|PointerMotionMask)};
__attribute__ ((hot,nonnull))
static void draw_outline(struct JBWMClient * c)
{
	if (!c->border)
		return;
	const uint8_t o = c->opt.no_title_bar ? 0 : jbwm_get_font_height();
	const struct JBWMScreen * s = jbwm_get_screen(c);
	const struct JBWMRectangle * g = &c->size;
	const uint8_t b = c->border;
	XRectangle r = {g->x, g->y - o, g->width + b, g->height + b + o};
	XDrawRectangles(c->d, s->root, s->gc, &r, 1);
}
__attribute__((nonnull))
void jbwm_configure_client(struct JBWMClient * c)
{
	const jbwm_window_t w = c->window;
	struct JBWMRectangle * g = &c->size;
	XSendEvent(c->d, w, true, StructureNotifyMask, (XEvent *)
		&(XConfigureEvent){.x = g->x, .y = g->y, .width = g->width,
		.height = g->height, .type = ConfigureNotify, .event = w,
		.window = w, .above = c->parent, .override_redirect = true,
		.border_width = c->border});
}
static void grab_pointer(Display * d, const jbwm_window_t w)
{
	static Cursor c;
	if (!c)
		c = XCreateFontCursor(d, XC_fleur);
	XGrabPointer(d, w, false, JBWMMouseMask, GrabModeAsync,
		GrabModeAsync, None, c, CurrentTime);
}
static struct JBDim get_mouse_position(Display * d,
	const jbwm_window_t w)
{
	int x, y;
	XQueryPointer(d, w, &(Window){0}, &(Window){0},
		&x, &y, &(int){0}, &(int){0}, &(unsigned int){0});
	return (struct JBDim){.x = x, .y = y};
}
static void warp_corner(struct JBWMClient * c)
{
	XWarpPointer(c->d, None, c->window, 0, 0, 0, 0,
		c->size.width, c->size.height);
}
static void set_size(struct JBWMClient * c,
	const int16_t * p)
{
	struct JBWMRectangle * g = &c->size;
	g->width = abs(g->x - p[0]);
	g->height = abs(g->y - p[1]);
}
__attribute__((const))
static int16_t get_diff_factored(const int16_t a, const int16_t b,
	const int16_t c)
{
	return a - b + c;
}
__attribute__((nonnull,pure))
static int16_t get_diff(const uint8_t i, const int16_t * original,
	const int16_t * start, const int16_t * p)
{
	return get_diff_factored(original[i], start[i], p[i]);
}
static void set_position(struct JBWMClient * c,
	const int16_t * original, const int16_t * start,
	const int16_t * p)
{
	c->size.x = get_diff(0, original, start, p);
	c->size.y = get_diff(1, original, start, p);
	jbwm_snap_client(c);
}
static void do_changes(struct JBWMClient * c, const bool resize,
	const int16_t * start, const int16_t * original,
	const int16_t * p)
{
	if (resize)
		set_size(c, p);
	else // drag
		set_position(c, original, start, p);
}
struct JBWMScreen * jbwm_get_screen(struct JBWMClient * c)
{
	return jbwm_get_screens() + c->screen;
}
jbwm_window_t jbwm_get_root(struct JBWMClient * c)
{
	return jbwm_get_screen(c)->root;
}
static void drag_event_loop(struct JBWMClient * c, const bool resize)
{
	Display * d = c->d;
	int16_t start[2];
	{ // p scope
		const struct JBDim p = get_mouse_position(d, jbwm_get_root(c));
		start[0] = p.x;
		start[1] = p.y;
	}
	const int16_t original[2] = {c->size.x, c->size.y};
	for (;;) {
		int16_t p[2];
		{ // e scope
			XEvent e;
			XMaskEvent(d, JBWMMouseMask, &e);
			if (e.type != MotionNotify)
				return;
			p[0] = e.xmotion.x;
			p[1] = e.xmotion.y;
		}
		draw_outline(c);
		do_changes(c, resize, start, original, p);
		if (c->border)
			draw_outline(c);
		else
			jbwm_move_resize(c);
	}
}
void jbwm_drag(struct JBWMClient * c, const bool resize)
{
	Display * d = c->d;
	XRaiseWindow(d, c->parent);
	if (resize && (c->opt.no_resize || c->opt.shaded))
		return;
	grab_pointer(d, jbwm_get_root(c));
	if (resize)
		warp_corner(c);
	drag_event_loop(c, resize);
	draw_outline(c);
	XUngrabPointer(d, CurrentTime);
	jbwm_move_resize(c);
}
void jbwm_move_resize(struct JBWMClient * c)
{
	JBWM_LOG("jbwm_move_resize");
	const uint8_t offset = c->opt.no_title_bar || c->opt.fullscreen
		? 0 : jbwm_get_font_height();
	{ // * d , * g scope
		Display * d = c->d;
		struct JBWMRectangle * g = &c->size;
		XMoveResizeWindow(d, c->parent, g->x, g->y - offset, g->width,
			g->height + offset);
		XMoveResizeWindow(d, c->window, 0, offset,
			g->width, g->height);
	}
	if(offset) { // Leave braces in case title bar support was disabled.
		jbwm_update_title_bar(c);
	} // Skip shaped and fullscreen clients.
	jbwm_set_shape(c);
	jbwm_set_frame_extents(c);
	jbwm_configure_client(c);
}
static void showing(struct JBWMClient * c,
	int (* mapping)(Display *, Window),
	void (* ewmh_add_remove_state)(Display * restrict, jbwm_window_t,
	jbwm_atom_t), const int8_t wm_state)
{
	Display * d = c->d;
	mapping(d, c->parent);
	jbwm_set_wm_state(c, wm_state);
	ewmh_add_remove_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_HIDDEN));
}
void jbwm_hide_client(struct JBWMClient * c)
{
	showing(c, XUnmapWindow, jbwm_ewmh_add_state, IconicState);
}
void jbwm_restore_client(struct JBWMClient * c)
{
	showing(c, XMapWindow, jbwm_ewmh_remove_state, NormalState);
}
static void check_visibility(struct JBWMScreen * s,
	struct JBWMClient * c, const uint8_t v)
{
	if (c->screen != s->screen)
		return;
	if (c->vdesk == v || c->opt.sticky) {
		c->vdesk = v; // allow moving windows by sticking
		jbwm_restore_client(c);
	} else
		jbwm_hide_client(c);
}
uint8_t jbwm_set_vdesk(struct JBWMScreen * s, uint8_t v)
{
	JBWM_LOG("jbwm_set_vdesk");
	if (v == s->vdesk || v > JBWM_MAX_DESKTOPS)
		return s->vdesk;
	for (struct JBWMClient * c = jbwm_get_head_client();
		c; c = c->next)
		check_visibility(s, c, v);
	s->vdesk = v;
#ifdef JBWM_USE_EWMH
	jbwm_set_property(s->d, s->root,
		jbwm_ewmh_get_atom(JBWM_EWMH_CURRENT_DESKTOP),
		XA_CARDINAL, &v, 1);
#endif//JBWM_USE_EWMH
	return s->vdesk;
}
