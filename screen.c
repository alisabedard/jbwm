// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "screen.h"
#include "client.h"
#include "config.h"
#include "ewmh_state.h"
#include "ewmh.h"
#include "font.h"
#include "jbwm.h"
#include "log.h"
#include "shape.h"
#include "snap.h"
#include "title_bar.h"
#include "util.h"
#include <stdlib.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
//#define DEBUG_SCREEN
#ifndef DEBUG_SCREEN
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!DEBUG_SCREEN
enum {JBWMMouseMask=(ButtonPressMask|ButtonReleaseMask|PointerMotionMask)};
__attribute__ ((hot,nonnull))
static void draw_outline(Display * restrict d, struct JBWMClient * restrict c)
{
	if (!c->border)
		return;
	const uint8_t offset = c->opt.no_title_bar
		? 0 : jbwm_get_font_height();
	struct JBWMScreen * s = &jbwm_get_screens()[c->screen];
	XDrawRectangle(d, s->root, s->gc, c->size.x, c->size.y - offset,
		c->size.width + c->border, c->size.height + c->border
		+ offset);
}
__attribute__((nonnull))
static void configure(Display * restrict d,
	struct JBWMRectangle * restrict g,
	const jbwm_window_t w)
{
	XSendEvent(d, w, true, StructureNotifyMask, (XEvent *)
		&(XConfigureEvent){.x = g->x, .y = g->y, .width = g->width,
		.height = g->height, .type = ConfigureNotify, .event = w });
}
static void grab_pointer(Display * restrict d, const jbwm_window_t w)
{
	static Cursor c;
	if (!c)
		c = XCreateFontCursor(d, XC_fleur);
	XGrabPointer(d, w, false, JBWMMouseMask, GrabModeAsync,
		GrabModeAsync, None, c, CurrentTime);
}
static jbwm_point_t get_mouse_position(Display * restrict d,
	const jbwm_window_t w)
{
	int x, y;
	XQueryPointer(d, w, &(Window){0}, &(Window){0},
		&x, &y, &(int){0}, &(int){0}, &(unsigned int){0});
	return (jbwm_point_t){x, y};
}
static void warp_corner(Display * restrict d, struct JBWMClient * restrict c)
{
	XWarpPointer(d, None, c->window, 0, 0, 0, 0,
		c->size.width, c->size.height);
}
static void set_size(struct JBWMClient * restrict c,
	const int16_t x, const int16_t y)
{
	c->size.width = abs(c->size.x - x);
	c->size.height = abs(c->size.y - y);
}
static void set_position(struct JBWMClient * restrict c,
	jbwm_point_t old, jbwm_point_t start,
	const int16_t x, const int16_t y)
{
	c->size.x = old.x - start.x + x;
	c->size.y = old.y - start.y + y;
	jbwm_snap_client(c);
}
static void do_changes(struct JBWMClient * restrict c, const bool resize,
	const jbwm_point_t start, const jbwm_point_t original,
	const int16_t x, const int16_t y)
{
	if (resize)
		set_size(c, x, y);
	else // drag
		set_position(c, original, start, x, y);
}
static void drag_event_loop(Display * restrict d,
	struct JBWMClient * restrict c, const bool resize)
{
	const jbwm_point_t start = get_mouse_position(d,
		jbwm_get_screens()[c->screen].root);
	const jbwm_point_t original = {c->size.x, c->size.y};
	for (;;) {
		{ // e scope
			XEvent e;
			XMaskEvent(d, JBWMMouseMask, &e);
			if (e.type != MotionNotify)
				return;
			draw_outline(d, c);
			do_changes(c, resize, start, original,
				e.xmotion.x, e.xmotion.y);
		}
		if (c->border)
			draw_outline(d, c);
		else
			jbwm_move_resize(d, c);
	}
}
void jbwm_drag(Display * restrict d, struct JBWMClient * restrict c,
	const bool resize)
{
	XRaiseWindow(d, c->parent);
	if (resize && (c->opt.no_resize || c->opt.shaded))
		return;
	grab_pointer(d, jbwm_get_screens()[c->screen].root);
	if (resize)
		warp_corner(d, c);
	drag_event_loop(d, c, resize);
	draw_outline(d, c);
	XUngrabPointer(d, CurrentTime);
	jbwm_move_resize(d, c);
	if (!resize && !c->opt.tearoff)
		configure(d, (&c->size), c->window);
}
void jbwm_move_resize(Display * restrict d, struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_move_resize");
	const uint8_t offset = c->opt.no_title_bar || c->opt.fullscreen
		? 0 : jbwm_get_font_height();
	XMoveResizeWindow(d, c->parent,
		c->size.x, c->size.y - offset,
		c->size.width, c->size.height + offset);
	XMoveResizeWindow(d, c->window, 0, offset,
		c->size.width, c->size.height);
	if(offset) { // Leave braces in case title bar support was disabled.
		jbwm_update_title_bar(d, c);
	} // Skip shaped and fullscreen clients.
	jbwm_set_shape(d, c);
}
static void hide(Display * restrict d, struct JBWMClient * restrict c)
{
	XUnmapWindow(d, c->parent);
	jbwm_set_wm_state(d, c, IconicState);
	jbwm_ewmh_add_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_HIDDEN));
}
void jbwm_restore_client(Display * restrict d, struct JBWMClient * restrict c)
{
	XMapWindow(d, c->parent);
	jbwm_set_wm_state(d, c, NormalState);
	jbwm_ewmh_remove_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_HIDDEN));
}
static void check_visibility(Display * restrict d, struct JBWMScreen * s,
	struct JBWMClient * restrict c, const uint8_t v)
{
	if (c->screen != s->screen)
		return;
	if (c->vdesk == v || c->opt.sticky) {
		c->vdesk = v; // allow moving windows by sticking
		jbwm_restore_client(d, c);
	} else
		hide(d, c);
}
uint8_t jbwm_set_vdesk(Display * restrict d, struct JBWMScreen * s, uint8_t v)
{
	JBWM_LOG("jbwm_set_vdesk");
	if (v == s->vdesk || v > JBWM_MAX_DESKTOPS)
		return s->vdesk;
	for (struct JBWMClient * c = jbwm_get_head_client();
		c; c = c->next)
		check_visibility(d, s, c, v);
	s->vdesk = v;
#ifdef JBWM_USE_EWMH
	jbwm_set_property(d, s->root,
		jbwm_ewmh_get_atom(JBWM_EWMH_CURRENT_DESKTOP),
		XA_CARDINAL, &v, 1);
#endif//JBWM_USE_EWMH
	return s->vdesk;
}
