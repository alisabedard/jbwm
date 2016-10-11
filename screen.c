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

enum {MouseMask=(ButtonPressMask|ButtonReleaseMask|PointerMotionMask)};

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
	XGrabPointer(jbwm.d, w, false, MouseMask, GrabModeAsync,
		GrabModeAsync, None, jbwm.cursor, CurrentTime);
}

static jbwm_point_t get_mouse_position(jbwm_window_t w)
{
	int x, y;
	XQueryPointer(jbwm.d, w, &(Window){0}, &(Window){0},
		&x, &y, &(int){0}, &(int){0}, &(unsigned int){0});
	return (jbwm_point_t){x, y};
}

void jbwm_drag(struct JBWMClient * restrict c, const bool resize)
{
	XRaiseWindow(jbwm.d, c->parent);
	if (resize && (c->opt.no_resize || c->opt.shaded))
		return;
	grab_pointer(c->screen->root);
	const jbwm_point_t p = get_mouse_position(c->screen->root);
	const jbwm_point_t op = {c->size.x, c->size.y};
	XEvent e;
	if (resize)
		XWarpPointer(jbwm.d, None, c->window, 0, 0, 0, 0,
			c->size.width, c->size.height);
	for(;;) {
		XMaskEvent(jbwm.d, MouseMask, &e);
		if (e.type != MotionNotify)
			break;
		draw_outline(c);
		if (resize) {
			c->size.width=abs(c->size.x - e.xmotion.x);
			c->size.height=abs(c->size.y - e.xmotion.y);
		} else {
			c->size.x = op.x - p.x + e.xmotion.x;
			c->size.y = op.y - p.y + e.xmotion.y;
			snap_client(c);
		}
		(c->border ? draw_outline : moveresize)(c);
	}
	draw_outline(c);
	XUngrabPointer(jbwm.d, CurrentTime);
	moveresize(c);
	if (!resize && !c->opt.tearoff)
		configure((&c->size), c->window);
}

void moveresize(struct JBWMClient * restrict c)
{
	LOG("moveresize");
	const uint8_t offset = c->opt.no_titlebar || c->opt.fullscreen
		? 0 : TDIM;
	XMoveResizeWindow(jbwm.d, c->parent,
		c->size.x, c->size.y - offset,
		c->size.width, c->size.height + offset);
	XMoveResizeWindow(jbwm.d, c->window,
		0, offset,
		c->size.width, c->size.height);
	if(offset) { update_titlebar(c); } // Avoid shaped and fullscreen
	set_shape(c);
}

static void hide(struct JBWMClient * restrict c)
{
	XUnmapWindow(jbwm.d, c->parent);
#ifdef EWMH
	ewmh_add_state(c->window, ewmh[WM_STATE_HIDDEN]);
#endif//EWMH
}

void unhide(struct JBWMClient * restrict c)
{
	XMapWindow(jbwm.d, c->parent);
#ifdef EWMH
	ewmh_remove_state(c->window, ewmh[WM_STATE_HIDDEN]);
#endif//EWMH
}

uint8_t switch_vdesk(struct JBWMScreen * s, uint8_t v)
{
	LOG("switch_vdesk");

	if (v == s->vdesk || v > DESKTOPS)
		return s->vdesk;

	for (struct JBWMClient * c = jbwm.head; c; c = c->next) {
		if (c->opt.sticky) {
			unhide(c);
			continue;
		}
		if (c->screen != s) continue;
		if (c->vdesk == s->vdesk) hide(c);
		else if (c->vdesk == v) unhide(c);
	}
	s->vdesk = v;
#ifdef EWMH
	XPROP(s->root, ewmh[CURRENT_DESKTOP], XA_CARDINAL, &v, 1);
#endif//EWMH
	return s->vdesk;
}
