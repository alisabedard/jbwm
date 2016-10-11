// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "screen.h"

#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "JBWMEnv.h"
#include "log.h"
#include "shape.h"
#include "snap.h"
#include "titlebar.h"
#include "util.h"

#include <stdlib.h>
#include <X11/Xatom.h>

enum {JBWMMouseMask=(ButtonPressMask|ButtonReleaseMask|PointerMotionMask)};

__attribute__ ((hot,nonnull))
static void draw_outline(struct JBWMClient * restrict c)
{
	if (!c->border)
		return;
	const uint8_t offset = c->opt.no_titlebar ? 0 : TDIM;
	XDrawRectangle(jbwm.d, c->screen->root, c->screen->gc,
		c->size.x, c->size.y - offset,
		c->size.width + c->border,
		c->size.height + c->border + offset);
}

__attribute__((nonnull))
static void configure(XSizeHints * restrict g, const jbwm_window_t w)
{
	XSendEvent(jbwm.d, w, true, StructureNotifyMask, (XEvent *)
		&(XConfigureEvent){.x = g->x, .y = g->y, .width = g->width,
		.height = g->height, .type = ConfigureNotify, .event = w });
}

static void grab_pointer(const jbwm_window_t w)
{
	XGrabPointer(jbwm.d, w, false, JBWMMouseMask, GrabModeAsync,
		GrabModeAsync, None, jbwm.cursor, CurrentTime);
}

static jbwm_point_t get_mouse_position(jbwm_window_t w)
{
	int x, y;
	XQueryPointer(jbwm.d, w, &(Window){0}, &(Window){0},
		&x, &y, &(int){0}, &(int){0}, &(unsigned int){0});
	return (jbwm_point_t){x, y};
}

static void warp_corner(struct JBWMClient * restrict c)
{
	XWarpPointer(jbwm.d, None, c->window, 0, 0, 0, 0,
		c->size.width, c->size.height);
}

static void set_size(struct JBWMClient * restrict c,
	const int16_t x, const int16_t y)
{
	c->size.width=abs(c->size.x - x);
	c->size.height=abs(c->size.y - y);
}

static void set_position(struct JBWMClient * restrict c,
	jbwm_point_t old, jbwm_point_t start,
	const int16_t x, const int16_t y)
{
	c->size.x = old.x - start.x + x;
	c->size.y = old.y - start.y + y;
	snap_client(c);
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

static void drag_event_loop(struct JBWMClient * restrict c, const bool resize)
{
	const jbwm_point_t start = get_mouse_position(c->screen->root);
	const jbwm_point_t original = {c->size.x, c->size.y};
	for (;;) {
		XEvent e;
		XMaskEvent(jbwm.d, JBWMMouseMask, &e);
		if (e.type != MotionNotify)
			return;
		draw_outline(c);
		do_changes(c, resize, start, original,
			e.xmotion.x, e.xmotion.y);
		(c->border ? draw_outline : jbwm_move_resize)(c);
	}
}

void jbwm_drag(struct JBWMClient * restrict c, const bool resize)
{
	XRaiseWindow(jbwm.d, c->parent);
	if (resize && (c->opt.no_resize || c->opt.shaded))
		return;
	grab_pointer(c->screen->root);
	if (resize)
		warp_corner(c);
	drag_event_loop(c, resize);
	draw_outline(c);
	XUngrabPointer(jbwm.d, CurrentTime);
	jbwm_move_resize(c);
	if (!resize && !c->opt.tearoff)
		configure((&c->size), c->window);
}

void jbwm_move_resize(struct JBWMClient * restrict c)
{
	LOG("jbwm_move_resize");
	const uint8_t offset = c->opt.no_titlebar || c->opt.fullscreen
		? 0 : TDIM;
	XMoveResizeWindow(jbwm.d, c->parent,
		c->size.x, c->size.y - offset,
		c->size.width, c->size.height + offset);
	XMoveResizeWindow(jbwm.d, c->window,
		0, offset,
		c->size.width, c->size.height);
	if(offset) { // Leave braces in case titlebar support was disabled.
		jbwm_update_titlebar(c);
	} // Skip shaped and fullscreen clients.
	set_shape(c);
}

static void hide(struct JBWMClient * restrict c)
{
	XUnmapWindow(jbwm.d, c->parent);
	jbwm_set_wm_state(c, IconicState);
	ewmh_add_state(c->window, ewmh[WM_STATE_HIDDEN]);
}

void jbwm_restore_client(struct JBWMClient * restrict c)
{
	XMapWindow(jbwm.d, c->parent);
	jbwm_set_wm_state(c, NormalState);
	ewmh_remove_state(c->window, ewmh[WM_STATE_HIDDEN]);
}

uint8_t jbwm_set_vdesk(struct JBWMScreen * s, uint8_t v)
{
	LOG("jbwm_set_vdesk");

	if (v == s->vdesk || v > JBWM_MAX_DESKTOPS)
		return s->vdesk;

	for (struct JBWMClient * c = jbwm.head; c; c = c->next) {
		if (c->opt.sticky) {
			jbwm_restore_client(c);
			continue;
		}
		if (c->screen != s) continue;
		if (c->vdesk == s->vdesk) hide(c);
		else if (c->vdesk == v) jbwm_restore_client(c);
	}
	s->vdesk = v;
#ifdef EWMH
	jbwm_set_property(s->root, ewmh[CURRENT_DESKTOP], XA_CARDINAL, &v, 1);
#endif//EWMH
	return s->vdesk;
}
