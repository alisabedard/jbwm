// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "screen.h"

#include "client.h"
#include "ewmh.h"
#include "jbwmenv.h"
#include "log.h"
#include "shape.h"
#include "snap.h"
#include "titlebar.h"
#include "util.h"

#include <assert.h>
#include <stdlib.h>
#include <X11/Xatom.h>

enum {MouseMask=(ButtonPressMask|ButtonReleaseMask|PointerMotionMask)};

__attribute__ ((hot))
static void draw_outline(Client * restrict c)
{
	const uint8_t offset = c->opt.no_titlebar ? 0 : TDIM;
	XDrawRectangle(jbwm.dpy, c->screen->root, c->screen->gc,
		c->size.x, c->size.y - offset,
		c->size.width + c->border,
		c->size.height + c->border + offset);
}

static void configure(XSizeHints * restrict g, const Window w)
{
	XSendEvent(jbwm.dpy, w, true, StructureNotifyMask, (XEvent *)
		&(XConfigureEvent){.x = g->x, .y = g->y, .width = g->width,
		.height = g->width, .type = ConfigureNotify, .event = w });
}

static inline void grab_pointer(const Window w)
{
	XGrabPointer(jbwm.dpy, w, false, MouseMask, GrabModeAsync,
		GrabModeAsync, None, jbwm.cursor, CurrentTime);
}

void resize(Client * restrict c)
{
	LOG("resize");
	if (c->opt.no_resize || c->opt.shaded)
		  return;
	grab_pointer(c->screen->root);
	XEvent ev;
	XWarpPointer(jbwm.dpy, None, c->window, 0, 0, 0, 0,
		c->size.width, c->size.height);
resize_loop:
	XMaskEvent(jbwm.dpy, MouseMask, &ev);
	if (ev.type == MotionNotify) {
		if(c->border) draw_outline(c);
		c->size.width=abs(c->size.x-ev.xmotion.x);
		c->size.height=abs(c->size.y-ev.xmotion.y);
		if(c->border) draw_outline(c);
		else moveresize(c);
		goto resize_loop;
	}
	XUngrabPointer(jbwm.dpy, CurrentTime);
	moveresize(c);
	configure(&c->size, c->window);
}

static XPoint get_mouse_position(Window w)
{
	int x, y, d;
	XQueryPointer(jbwm.dpy, w, &w, &w, &x, &y,
		&d, &d, &(unsigned int){0});
	return (XPoint){x, y};
}

void drag(Client * restrict c)
{
	LOG("drag");
	grab_pointer(c->screen->root);
	const XPoint op = { c->size.x, c->size.y};
	XPoint p = get_mouse_position(c->screen->root);
	XEvent ev;
drag_begin:
	XMaskEvent(jbwm.dpy, MouseMask, &ev);
	if(ev.type == MotionNotify) {
		if(c->border) draw_outline(c); // clear
		c->size.x = op.x - p.x + ev.xmotion.x;
		c->size.y = op.y - p.y + ev.xmotion.y;
		snap_client(c);
		if(c->border) draw_outline(c); // draw
		else moveresize(c);
		goto drag_begin;
	}
	if(c->border) draw_outline(c); // clear
	XUngrabPointer(jbwm.dpy, CurrentTime);
	moveresize(c);
	configure(&(c->size), c->window);
}

void moveresize(Client * restrict c)
{
	LOG("moveresize");
	const uint8_t offset = c->opt.no_titlebar || c->opt.fullscreen
		? 0 : TDIM;
	XMoveResizeWindow(jbwm.dpy, c->parent,
		c->size.x, c->size.y - offset,
		c->size.width, c->size.height + offset);
	XMoveResizeWindow(jbwm.dpy, c->window,
		0, offset,
		c->size.width, c->size.height);
	if(offset) { update_titlebar(c); }
	set_shape(c);
}

void restore_horz(Client * restrict c)
{
	LOG("restore_horz");
	assert(c);
	if (!c->opt.max_horz) return;
	c->opt.max_horz = false;
	c->size.x = c->old_size.x;
	c->size.width = c->old_size.width;
	ewmh_remove_state(c->window, ewmh[WM_STATE_MAXIMIZED_HORZ]);
}

void maximize_horz(Client * restrict c)
{
	LOG("maximize_horz");
	assert(c);
	if (c->opt.max_horz) return;
	c->old_size.x = c->size.x;
	c->old_size.width = c->size.width;
	c->size.x = 0;
	c->size.width = c->screen->size.w;
	ewmh_add_state(c->window, ewmh[WM_STATE_MAXIMIZED_HORZ]);
	c->opt.max_horz = true;
	// Offset if not fullscreen
	if (c->opt.fullscreen) {
		c->size.width -= c->border<<1;
	}
}
void restore_vert(Client * restrict c)
{
	LOG("restore_vert");
	assert(c);
	if (c->opt.max_vert) {
		c->opt.max_vert = false;
		c->size.y = c->old_size.y;
		c->size.height = c->old_size.height;
		ewmh_remove_state(c->window,
			ewmh[WM_STATE_MAXIMIZED_VERT]);
	}
}

void maximize_vert(Client * restrict c)
{
	LOG("maximize_vert");
	assert(c);
	if (c->opt.max_vert) return;
	c->old_size.y = c->size.y;
	c->old_size.height = c->size.height;
	c->size.y = 0;
	c->size.height = c->screen->size.h;
	ewmh_add_state(c->window, ewmh[WM_STATE_MAXIMIZED_VERT]);
	c->opt.max_vert = true;
	// Offset the titlebar if not fullscreen
	if (!c->opt.fullscreen) {
		c->size.y += TDIM+c->border;
		c->size.height -= TDIM+(c->border<<2);
		moveresize(c);
	}
}

void unset_maximized(Client * restrict c)
{
	LOG("unset_maximized");
	assert(c);
	// Don't restore a fullscreen window.
	if(!c->opt.maximized || c->opt.fullscreen)
		  return;
	restore_horz(c);
	restore_vert(c);
	c->opt.maximized = false;
}

void set_maximized(Client * restrict c)
{
	LOG("set_maximized");
	assert(c);
	if(c->opt.maximized) return; // Already maximized
	maximize_horz(c);
	maximize_vert(c);
	c->opt.maximized = true;
}

void unset_fullscreen(Client * restrict c)
{
	LOG("unset_fullscreen");
	assert(c);
	if(!c->opt.is_fullscreen) return;
	c->opt.fullscreen = false; // Reflects desired status
	restore_horz(c);
	restore_vert(c);
	XSetWindowBorderWidth(jbwm.dpy, c->parent, c->border);
	ewmh_remove_state(c->window, ewmh[WM_STATE_FULLSCREEN]);
	update_titlebar(c);
	c->opt.is_fullscreen = false; // Reflects current status
}

void set_fullscreen(Client * restrict c)
{
	LOG("set_fullscreen");
	assert(c);
	if(c->opt.is_fullscreen) return; // Already fullscreen
	/* The following checks remove conflicts between fullscreen
	   mode and maximized modes.  */
	if(c->opt.maximized) unset_maximized(c);
	if(c->opt.max_horz) restore_horz(c);
	if(c->opt.max_vert) restore_vert(c);
	c->opt.fullscreen = true; // Reflect desired status
	maximize_horz(c);
	maximize_vert(c);
	XSetWindowBorderWidth(jbwm.dpy, c->parent, 0);
	ewmh_add_state(c->window, ewmh[WM_STATE_FULLSCREEN]);
	update_titlebar(c);
	c->opt.is_fullscreen = true; // Reflect current status
}

static void hide(Client * restrict c)
{
	LOG("hide");
	assert(c);
	/* This will generate an unmap event.  Tell event handler
	 * to ignore it.  */
	c->ignore_unmap++;
	XUnmapWindow(jbwm.dpy, c->parent);
	set_wm_state(c, IconicState);
}

void unhide(Client * restrict c)
{
	LOG("unhide");
	assert(c);
	XMapWindow(jbwm.dpy, c->parent);
	set_wm_state(c, NormalState);
}

uint8_t switch_vdesk(ScreenInfo * s, uint8_t v)
{
	LOG("switch_vdesk");
	assert(s);

	if (v == s->vdesk || v > DESKTOPS)
		return s->vdesk;

	for (Client * c = jbwm.head; c; c = c->next) {
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
	XPROP(s->root, ewmh[CURRENT_DESKTOP],
		XA_CARDINAL, &v, 1);
#endif//EWMH
	return s->vdesk;
}
