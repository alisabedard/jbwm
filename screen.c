// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

#define MouseMask (ButtonPressMask|ButtonReleaseMask|PointerMotionMask)

#ifndef SOLID
__attribute__ ((hot))
static void draw_outline(Client * c)
{
	assert(c);
	/* restrict, as all changes will occur through g */
	XSizeHints *restrict g = &(c->size);
	const bool tb = !(c->flags & JB_NO_TB);
/* *INDENT-OFF* */
	XRectangle r[] = { 
		{ .x = g->x, .y = g->y, .width = g->width, 
			.height = g->height}, 
		{ .x = g->x, .y = g->y - (tb ? TDIM : 0), 
			.width = g->width,.height = (tb ? TDIM : 0) - c->border}
	};
/* *INDENT-ON* */
	ScreenInfo *s = c->screen;
	XDrawRectangles(D, s->root, s->gc, r, tb ? 2 : 1);
}
#endif//!SOLID

static void configure(XSizeHints * restrict g, const Window w)
{
	XConfigureEvent ce = {.x = g->x,.y = g->y,.width = g->width,
		.height = g->height,.type = ConfigureNotify,.event = w
	};
	XSendEvent(D, w, true, StructureNotifyMask, (XEvent *) & ce);
}

static void recalculate_resize(XSizeHints * restrict g, XPoint p1, XPoint p2)
{
	LOG("recalculate_resize");
	g->width = abs(p1.x - p2.x);
	g->height = abs(p1.y - p2.y);
	g->x = p1.x;
	g->y = p1.y;
}

static bool grab_pointer(const Window w, const Cursor cursor)
{
	return XGrabPointer(D, w, false, MouseMask, GrabModeAsync,
			    GrabModeAsync, None, cursor,
			    CurrentTime) == GrabSuccess;
}

static void
handle_motion_notify(Client * restrict c, XMotionEvent * restrict mev)
{
#ifndef SOLID

	if (c->border)
		draw_outline(c);	// clear outline

#endif//!SOLID
/* *INDENT-OFF* */
	recalculate_resize(&c->size, (XPoint) { c->size.x, c->size.y},
		(XPoint) { mev->x, mev->y});
/* *INDENT-ON* */
#ifndef SOLID

	if (c->border)
		draw_outline(c);
	else
#endif//!SOLID
		moveresize(c);
}

void resize(Client * restrict c)
{
	LOG("resize");
	assert(c);
	/* Resizing shaded windows yields undefined behavior.  */
	const uint32_t f = c->flags;

	if (f & (JB_NO_RESIZE | JB_SHADED))
		return;

	if (!grab_pointer(c->screen->root, jbwm.cursor))
		return;

	// Valid with restricted c, as it is derived from c:
	XSizeHints *g = &(c->size);
#ifdef USE_SHAPE

	if (f & JB_SHAPED)	// Bugfix for offset issue
		g->height += TDIM;

#endif//USE_SHAPE
	XWarpPointer(D, None, c->window, 0, 0, 0, 0, g->width, g->height);
	XEvent ev;
#ifndef SOLID

	if (c->border)
		XGrabServer(D);

#endif//!SOLID
 resize_loop:
	XMaskEvent(D, MouseMask, &ev);

	if (ev.type == MotionNotify) {
		handle_motion_notify(c, &ev.xmotion);
		goto resize_loop;
	}

	configure(g, c->window);
#ifndef SOLID

	if (c->border)
		XUngrabServer(D);

#endif//!SOLID
	XUngrabPointer(D, CurrentTime);
	moveresize(c);
}

static void
drag_motion(Client * restrict c, XEvent ev, const XPoint p, const XPoint oldp)
{
	XSizeHints *g = &(c->size);
#ifndef SOLID
	const bool outline = c->border && !(c->flags & JB_SHADED);

	if (outline)
		draw_outline(c);

#endif//!SOLID
	g->x = oldp.x + (ev.xmotion.x - p.x);
	g->y = oldp.y + (ev.xmotion.y - p.y);
	snap_client(c);
#ifndef SOLID

	if (outline)
		draw_outline(c);
	else
#endif//!SOLID
		moveresize(c);
}

static void drag_event_loop(Client * restrict c, XPoint p, const XPoint oldp)
{
	XEvent ev;
#ifdef USE_TBAR
	p.y += c->flags & JB_NO_TB ? TDIM : 0;
#endif//USE_TBAR
	XGrabServer(D);
 drag_loop:
	XMaskEvent(D, MouseMask, &ev);

	if (ev.type == MotionNotify)
		drag_motion(c, ev, p, oldp);
	else {
		XUngrabPointer(D, CurrentTime);
		moveresize(c);
		XUngrabServer(D);
		configure(&(c->size), c->window);
		return;
	}

	goto drag_loop;
}

static inline XPoint get_mouse_position(Window w)
{
	XPoint p;
	XQueryPointer(D, w, &w, &w, (int *)&p.x, (int *)&p.y, (int *)&w,
		      (int *)&w, (unsigned int *)&w);
	return p;
}

void drag(Client * restrict c)
{
	LOG("drag");
	assert(c);

	if (c->flags & JB_NO_MOVE)
		return;

	const Window root = c->screen->root;

	if (!grab_pointer(root, jbwm.cursor))
		return;

	XSizeHints *g = &(c->size);
	drag_event_loop(c, get_mouse_position(root), (XPoint) {
			g->x, g->y}
	);
}

void moveresize(Client * restrict c)
{
#ifdef USE_TBAR
	const uint8_t offset =
	    (c->flags & (JB_NO_TB | JB_MAXIMIZED)) ? 0 : TDIM;
#else//!USE_TBAR
#define offset 0
#endif//USE_TBAR
	const uint16_t w = c->size.width;
	XMoveResizeWindow(D, c->parent, c->size.x, c->size.y - offset, w,
			  c->size.height + offset);
	XMoveResizeWindow(D, c->window, 0, offset, w, c->size.height);
#ifdef USE_TBAR

	if (offset && (c->exposed_width != w)) {
		update_titlebar(c);
		c->exposed_width = w;
	}
#endif//USE_TBAR
	set_shape(c);
}

void restore_horz(Client * restrict c)
{
	if (c->flags & JB_MAX_HORZ) {
		c->flags^=JB_MAX_HORZ;
		c->size.x=c->old_size.x;
		c->size.width=c->old_size.width;
		ewmh_remove_state(c->window, ewmh.WM_STATE_MAXIMIZED_HORZ);
	}
}

void maximize_horz(Client * restrict c)
{
	if (c->flags & JB_MAX_HORZ)
 		 return; 
	c->old_size.x=c->size.x;
	c->old_size.width=c->size.width;
	c->size.x=0;
	c->size.width=c->screen->size.w;
	ewmh_add_state(c->window, ewmh.WM_STATE_MAXIMIZED_HORZ);
	c->flags|=JB_MAX_HORZ;
}

void restore_vert(Client * restrict c)
{
	if (c->flags & JB_MAX_VERT) {
		c->flags^=JB_MAX_VERT;
		c->size.y=c->old_size.y;
		c->size.height=c->old_size.height;
		ewmh_remove_state(c->window, ewmh.WM_STATE_MAXIMIZED_VERT);
	}

}

void maximize_vert(Client * restrict c)
{
	if (c->flags & JB_MAX_VERT)
 		 return; 
	c->old_size.y=c->size.y;
	c->old_size.height=c->size.height;
	c->size.y=0;
	c->size.height=c->screen->size.h;
	ewmh_add_state(c->window, ewmh.WM_STATE_MAXIMIZED_VERT);
	c->flags|=JB_MAX_VERT;

}

void maximize(Client * restrict c)
{
	// Honor !MWM_FUNC_MAXIMIZE
	// Maximizing shaped windows is buggy, so return.
	if (c->flags & (JB_NO_MAX | JB_SHAPED))
		return;

	if (c->flags & JB_MAXIMIZED) {	// restore:
		restore_horz(c);
		restore_vert(c);
		XSetWindowBorderWidth(D, c->parent, c->border);
		ewmh_remove_state(c->window, ewmh.WM_STATE_FULLSCREEN);
	} else {			// maximize:
		maximize_horz(c);
		maximize_vert(c);
		XSetWindowBorderWidth(D, c->parent, 0);
		ewmh_add_state(c->window, ewmh.WM_STATE_FULLSCREEN);
	}
	c->flags ^= JB_MAXIMIZED;	// toggle
	update_titlebar(c);
	XRaiseWindow(D, c->parent);
	moveresize(c);
}

void hide(Client * restrict c)
{
	LOG("hide");
	assert(c);
	/* This will generate an unmap event.  Tell event handler
	 * to ignore it.  */
	c->ignore_unmap++;
	XUnmapWindow(D, c->parent);
	set_wm_state(c, IconicState);
}

void unhide(Client * restrict c)
{
	LOG("unhide");
	assert(c);
	XMapWindow(D, c->parent);
	set_wm_state(c, NormalState);
}

uint8_t switch_vdesk(ScreenInfo * s, const uint8_t v)
{
	LOG("switch_vdesk");
	assert(s);

	if (v == s->vdesk || v > DESKTOPS)
		return s->vdesk;

	for (Client * c = jbwm.head; c; c = c->next) {
		if (c->flags & JB_STICKY) {
			unhide(c);
			continue;
		}

		if (c->screen != s)
			continue;

		if (c->vdesk == s->vdesk)
			hide(c);
		else if (c->vdesk == v)
			unhide(c);
	}

	s->vdesk = v;
#ifdef EWMH
	XPROP(s->root, ewmh.CURRENT_DESKTOP, XA_CARDINAL, &(s->vdesk), 1);
#endif//EWMH
	return s->vdesk;
}
