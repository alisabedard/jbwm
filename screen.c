// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <assert.h>
#include <stdlib.h>
#include "atoms.h"
#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "jbwm.h"
#include "log.h"
#include "titlebar.h"
#include "screen.h"
#include "snap.h"

#define MouseMask (ButtonPressMask|ButtonReleaseMask|PointerMotionMask)

__attribute__ ((hot))
static void draw_outline(Client * c)
{
	/* restrict, as all changes will occur through g */
	XSizeHints *restrict g = &(c->size);
	const bool tb = !(c->flags & JB_NO_TB);
	XDrawRectangle(jbwm.dpy, c->screen->root, c->screen->gc,
		g->x, g->y - (tb?TDIM:0), g->width + c->border, 
		g->height + c->border + (tb?TDIM:0));
}

static void configure(XSizeHints * restrict g, const Window w)
{
	XSendEvent(jbwm.dpy, w, true, StructureNotifyMask, 
		(XEvent *) &(XConfigureEvent){.x = g->x,.y = g->y,
			.width = g->width, .height = g->height,
			.type = ConfigureNotify,.event = w });
}

static void recalculate_resize(XSizeHints * restrict g, XPoint p1, XPoint p2)
{
	LOG("recalculate_resize");
	g->width = abs(p1.x - p2.x);
	g->height = abs(p1.y - p2.y);
	g->x = p1.x;
	g->y = p1.y;
}

static inline bool grab_pointer(const Window w, const Cursor cursor)
{
	return XGrabPointer(jbwm.dpy, w, false, MouseMask, GrabModeAsync,
			    GrabModeAsync, None, cursor,
			    CurrentTime) == GrabSuccess;
}

static void
handle_motion_notify(Client * restrict c, XMotionEvent * restrict mev)
{
	if (c->border) draw_outline(c);	// clear outline

	recalculate_resize(&c->size, (XPoint) { c->size.x, c->size.y},
		(XPoint) { mev->x, mev->y});

	if (c->border) draw_outline(c);
	else moveresize(c);
}

void resize(Client * restrict c)
{
	LOG("resize");
	assert(c);
	/* Resizing shaded windows yields undefined behavior.  */
	const uint32_t f = c->flags;

	if (f & (JB_NO_RESIZE | JB_SHADED)) return;
	if (!grab_pointer(c->screen->root, jbwm.cursor)) return;

	// Valid with restricted c, as it is derived from c:
	XSizeHints *g = &(c->size);
#ifdef USE_SHAPE

	if (f & JB_SHAPED)	// Bugfix for offset issue
		g->height += TDIM;

#endif//USE_SHAPE
	XWarpPointer(jbwm.dpy, None, c->window, 0, 0, 0, 0, g->width, g->height);
	XEvent ev;
	if (c->border) XGrabServer(jbwm.dpy);
 resize_loop:
	XMaskEvent(jbwm.dpy, MouseMask, &ev);

	if (ev.type == MotionNotify) {
		handle_motion_notify(c, &ev.xmotion);
		goto resize_loop;
	}

	configure(g, c->window);
	if (c->border) XUngrabServer(jbwm.dpy);
	XUngrabPointer(jbwm.dpy, CurrentTime);
	moveresize(c);
}

static XPoint get_mouse_position(Window w)
{
	XPoint p;
	XQueryPointer(jbwm.dpy, w, &w, &w, (int *)&p.x, (int *)&p.y, (int *)&w,
		      (int *)&w, (unsigned int *)&w);
	return p;
}

void drag(Client * restrict c)
{
	LOG("drag");
	assert(c);
	assert(c->screen);
	const Window r = c->screen->root;
	if(!grab_pointer(r, jbwm.cursor))
		  return;
	const XPoint op = { c->size.x, c->size.y 
		+ (c->flags & JB_NO_TB ? TDIM : 0)};
	XPoint p=get_mouse_position(r);
	p.y += c->flags & JB_NO_TB ? TDIM : 0;
	XEvent ev;
drag_begin:
	XMaskEvent(jbwm.dpy, MouseMask, &ev);
	if(ev.type != MotionNotify)
		  goto drag_end;
	draw_outline(c); // clear
	c->size.x = op.x - p.x + ev.xmotion.x;
	c->size.y = op.y - p.y + ev.xmotion.y;
	snap_client(c);
	draw_outline(c); // draw
	goto drag_begin;
drag_end:
	draw_outline(c); // clear
	XUngrabPointer(jbwm.dpy, CurrentTime);
	moveresize(c);
	configure(&(c->size), c->window);
}

void moveresize(Client * restrict c)
{
	LOG("moveresize");
	assert(c);
#ifdef USE_TBAR
	const uint8_t offset =
	    (c->flags & (JB_NO_TB | JB_FULLSCREEN)) ? 0 : TDIM;
#else//!USE_TBAR
#define offset 0
#endif//USE_TBAR
	const uint16_t w = c->size.width;
	XMoveResizeWindow(jbwm.dpy, c->parent, c->size.x, c->size.y - offset, w,
			  c->size.height + offset);
	XMoveResizeWindow(jbwm.dpy, c->window, 0, offset, w, c->size.height);

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
	LOG("restore_horz");
	assert(c);
	if(!(c->flags & JB_MAX_HORZ))
		  return;
	c->flags^=JB_MAX_HORZ;
	c->size.x=c->old_size.x;
	c->size.width=c->old_size.width;
	ewmh_remove_state(c->window, ewmh.WM_STATE_MAXIMIZED_HORZ);
}

void maximize_horz(Client * restrict c)
{
	LOG("maximize_horz");
	assert(c);
	if (c->flags & JB_MAX_HORZ)
 		 return; 
	c->old_size.x=c->size.x;
	c->old_size.width=c->size.width;
	c->size.x=0;
	c->size.width=c->screen->size.w;
	ewmh_add_state(c->window, ewmh.WM_STATE_MAXIMIZED_HORZ);
	c->flags|=JB_MAX_HORZ;
	// Offset if not fullscreen
	if(!(c->flags&JB_FULLSCREEN)) {
		c->size.width-=2*c->border;
	}
}
void restore_vert(Client * restrict c)
{
	LOG("restore_vert");
	assert(c);
	if (c->flags & JB_MAX_VERT) {
		c->flags^=JB_MAX_VERT;
		c->size.y=c->old_size.y;
		c->size.height=c->old_size.height;
		ewmh_remove_state(c->window, ewmh.WM_STATE_MAXIMIZED_VERT);
	}
}

void maximize_vert(Client * restrict c)
{
	LOG("maximize_vert");
	assert(c);
	if (c->flags & JB_MAX_VERT)
 		 return; 
	c->old_size.y=c->size.y;
	c->old_size.height=c->size.height;
	c->size.y=0;
	c->size.height=c->screen->size.h;
	ewmh_add_state(c->window, ewmh.WM_STATE_MAXIMIZED_VERT);
	c->flags|=JB_MAX_VERT;
	// Offset the titlebar if not fullscreen
	if(!(c->flags&JB_FULLSCREEN)) {
		const uint8_t b = c->border;
		c->size.y+=TDIM+b;
		c->size.height-=TDIM+4*b;
		moveresize(c);
	}
}

void unset_maximized(Client * restrict c)
{
	LOG("unset_maximized");
	assert(c);
	// Don't restore a fullscreen window.  
	if(!(c->flags&JB_MAXIMIZED)||(c->flags&JB_FULLSCREEN))
		  return;
	restore_horz(c);
	restore_vert(c);
	c->flags &= ~JB_MAXIMIZED;
}

void set_maximized(Client * restrict c)
{
	LOG("set_maximized");
	assert(c);
	if(c->flags&JB_MAXIMIZED)
		  return;
	maximize_horz(c);
	maximize_vert(c);
	c->flags |= JB_MAXIMIZED;
}

void maximize(Client * restrict c)
{
	LOG("maximize");
	assert(c);
	// Honor !MWM_FUNC_MAXIMIZE
	// Maximizing shaped windows is buggy, so return.
	if (!(c->flags & (JB_NO_MAX | JB_SHAPED)))
		c->flags&JB_MAXIMIZED?unset_maximized(c):set_maximized(c);
}

void unset_fullscreen(Client * restrict c)
{
	LOG("unset_fullscreen");
	assert(c);
	if(!(c->flags&JB_IS_FS))
		  return;
	c->flags &= ~JB_FULLSCREEN;
	restore_horz(c);
	restore_vert(c);
	XSetWindowBorderWidth(jbwm.dpy, c->parent, c->border);
	ewmh_remove_state(c->window, ewmh.WM_STATE_FULLSCREEN);
	update_titlebar(c);
	c->flags &= ~JB_IS_FS;
}

void set_fullscreen(Client * restrict c)
{
	LOG("set_fullscreen");
	assert(c);
	if(c->flags&JB_IS_FS)
		  return;
	/* The following checks remove conflicts between fullscreen
	   mode and maximized modes.  */
	if(c->flags&JB_MAXIMIZED)
		  unset_maximized(c);
	if(c->flags&JB_MAX_HORZ)
		  restore_horz(c);
	if(c->flags&JB_MAX_VERT)
		  restore_vert(c);
	c->flags |= JB_FULLSCREEN;
	maximize_horz(c);
	maximize_vert(c);
	XSetWindowBorderWidth(jbwm.dpy, c->parent, 0);
	ewmh_add_state(c->window, ewmh.WM_STATE_FULLSCREEN);
	update_titlebar(c);
	c->flags |= JB_IS_FS;
}

void fullscreen(Client * restrict c)
{
	assert(c);
	if(c->flags&JB_NO_MAX)
		  return;
	if(c->flags&JB_IS_FS)
		  unset_fullscreen(c);
	else
		  set_fullscreen(c);
}

void hide(Client * restrict c)
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
		if (c->screen != s) continue;
		if (c->vdesk == s->vdesk) hide(c);
		else if (c->vdesk == v) unhide(c);
	}

	s->vdesk = v;
#ifdef EWMH
	XPROP(s->root, ewmh.CURRENT_DESKTOP, XA_CARDINAL, &(s->vdesk), 1);
#endif//EWMH
	return s->vdesk;
}
