// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "screen.h"
#include <stdlib.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include "JBWMDefaults.h"
#include "JBWMScreen.h"
#include "client.h"
#include "config.h"
#include "display.h"
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
static void draw_outline(Display * d, struct JBWMClient * restrict c)
{
	const uint8_t o = c->opt.no_title_bar ? 0 : jbwm_get_font_height();
	const struct JBWMScreen * s = jbwm_get_screen(c);
	const struct JBWMRectangle * g = &c->size;
	const uint8_t b = c->border;
	XRectangle r = {g->x, g->y - o, g->width + b, g->height + b + o};
	XDrawRectangles(d, s->root, s->gc, &r, 1);
}
__attribute__((nonnull))
void jbwm_configure_client(struct JBWMClient * restrict c)
{
	const Window w = c->window;
	struct JBWMRectangle * g = &c->size;
	XSendEvent(jbwm_get_display(), w, true, StructureNotifyMask, (XEvent *)
		&(XConfigureEvent){.x = g->x, .y = g->y, .width = g->width,
		.height = g->height, .type = ConfigureNotify, .event = w,
		.window = w, .above = c->parent, .override_redirect = true,
		.border_width = c->border});
}
static void grab_pointer(Display * d, const Window w)
{
	static Cursor c;
	if (!c)
		c = XCreateFontCursor(d, XC_fleur);
	XGrabPointer(d, w, false, JBWMMouseMask, GrabModeAsync,
		GrabModeAsync, None, c, CurrentTime);
}
static int * get_mouse_position(Display * restrict d, Window w)
{
	static int p[2];
	int nil;
	unsigned int unil;
	XQueryPointer(d, w, &w, &w, p, p + 1, &nil, &nil, &unil);
	return p;
}
static void set_size(struct JBWMClient * restrict c,
	const int * p)
{
	struct JBWMRectangle * g = &c->size;
	g->width = abs(g->x - p[0]);
	g->height = abs(g->y - p[1]);
}
__attribute__((const))
static int get_diff_factored(const int a, const int b,
	const int c)
{
	return a - b + c;
}
__attribute__((nonnull,pure))
static int get_diff(const uint8_t i, const int * original,
	const int * start, const int * p)
{
	return get_diff_factored(original[i], start[i], p[i]);
}
static void set_position(struct JBWMClient * restrict c,
	const int * original, const int * start,
	const int * p)
{
	c->size.x = get_diff(0, original, start, p);
	c->size.y = get_diff(1, original, start, p);
	jbwm_snap_client(c);
}
static void do_changes(struct JBWMClient * restrict c, const bool resize,
	const int * start, const int * original,
	const int * p)
{
	if (resize)
		set_size(c, p);
	else // drag
		set_position(c, original, start, p);
}
struct JBWMScreen * jbwm_get_screen(struct JBWMClient * restrict c)
{
	return jbwm_get_screens() + c->screen;
}
Window jbwm_get_root(struct JBWMClient * restrict c)
{
	return jbwm_get_screen(c)->root;
}
static void drag_event_loop(struct JBWMClient * restrict c, const bool resize)
{
	Display * d = jbwm_get_display();
	const int * start = get_mouse_position(d, jbwm_get_root(c)),
		original[] = {c->size.x, c->size.y};
	const uint8_t b = c->border;
	for (;;) {
		int p[2];
		{ // e scope
			XEvent e;
			XMaskEvent(d, JBWMMouseMask, &e);
			if (e.type != MotionNotify)
				return;
			p[0] = (int)e.xmotion.x;
			p[1] = (int)e.xmotion.y;
		}
		if (b)
			draw_outline(d, c);
		do_changes(c, resize, start, original, p);
		if (b)
			draw_outline(d, c);
		else
			jbwm_move_resize(c);
	}
}
static void warp_corner(Display * d, struct JBWMClient * restrict c)
{
	XWarpPointer(d, None, c->window, 0, 0, 0, 0,
		c->size.width, c->size.height);
}
void jbwm_drag(struct JBWMClient * restrict c, const bool resize)
{
	Display * d = jbwm_get_display();
	XRaiseWindow(d, c->parent);
	if (resize && (c->opt.no_resize || c->opt.shaded))
		return;
	grab_pointer(d, jbwm_get_root(c));
	if (resize)
		warp_corner(d, c);
	drag_event_loop(c, resize);
	if (c->border)
		draw_outline(d, c);
	XUngrabPointer(d, CurrentTime);
	jbwm_move_resize(c);
}
void jbwm_move_resize(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_move_resize");
	struct JBWMClientOptions * restrict o = &c->opt;
	const uint8_t offset = o->no_title_bar || o->fullscreen
		? 0 : jbwm_get_font_height();
	Display * d = jbwm_get_display();
	{ //* g scope
		struct JBWMRectangle * g = &c->size;
		XMoveResizeWindow(d, c->parent, g->x, g->y - offset,
			g->width, g->height + offset);
		XMoveResizeWindow(d, c->window, 0, offset,
			g->width, g->height);
	}
	if(offset) { // Leave braces in case title bar support was disabled.
		jbwm_update_title_bar(c);
	} // Skip shaped and fullscreen clients.
	jbwm_set_shape(c);
#if 0
	if (!o->frame_extents_set) {
		jbwm_set_frame_extents(c);
		o->frame_extents_set = true;
	}
#endif
	jbwm_configure_client(c);
}
#ifdef JBWM_USE_EWMH
static void showing(struct JBWMClient * restrict c,
	int (* mapping)(Display *, Window),
	void (* ewmh_add_remove_state)(Display * , Window,
	Atom), const int8_t wm_state)
{
	Display * d = jbwm_get_display();
	mapping(d, c->parent);
	jbwm_set_wm_state(c, wm_state);
	ewmh_add_remove_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_HIDDEN));
}
void jbwm_hide_client(struct JBWMClient * restrict c)
{
	showing(c, XUnmapWindow, jbwm_ewmh_add_state, IconicState);
}
void jbwm_restore_client(struct JBWMClient * restrict c)
{
	showing(c, XMapWindow, jbwm_ewmh_remove_state, NormalState);
}
#else//!JBWM_USE_EWMH
static void showing(struct JBWMClient * restrict c,
	int (* mapping)(Display *, Window),
	const int8_t wm_state)
{
	Display * d = jbwm_get_display();
	mapping(d, c->parent);
	jbwm_set_wm_state(c, wm_state);
}
void jbwm_hide_client(struct JBWMClient * restrict c)
{
	showing(c, XUnmapWindow, IconicState);
}
void jbwm_restore_client(struct JBWMClient * restrict c)
{
	showing(c, XMapWindow, NormalState);
}
#endif//JBWM_USE_EWMH
static void check_visibility(struct JBWMScreen * s,
	struct JBWMClient * restrict c, const uint8_t v)
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
	for (struct JBWMClient * restrict c = jbwm_get_head_client();
		c; c = c->next)
		check_visibility(s, c, v);
	s->vdesk = v;
#ifdef JBWM_USE_EWMH
	jbwm_set_property(jbwm_get_display(), s->root,
		jbwm_ewmh_get_atom(JBWM_EWMH_CURRENT_DESKTOP),
		XA_CARDINAL, &v, 1);
#endif//JBWM_USE_EWMH
	return s->vdesk;
}
