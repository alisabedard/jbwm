/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2016, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */
#include "jbwm.h"
#include <string.h>
#include "screen.h"

#define MouseMask (ButtonPressMask|ButtonReleaseMask|PointerMotionMask)

__attribute__((hot))
static inline void
draw_outline(Client * c)
{
	const bool no_tb=c->flags & JB_CLIENT_NO_TB;
	if(!c->border && !no_tb)
		return;
	ScreenInfo *s=c->screen;
	const Window r=s->root;
	XSizeHints *g=&(c->size);
	XRectangle d={.x=g->x, .width=g->width};
#ifdef USE_TBAR
	d.y=g->y-(no_tb?0:TDIM);
	d.height=g->height + (no_tb ? 0 : TDIM);
#else//!USE_TBAR
	d.y=g->y;
	d.height=g->height;
#endif//USE_TBAR
	XDrawRectangle(D, r, s->gc, d.x, d.y, d.width, d.height);
}

static inline void
recalculate_sweep(Client * c, Position p1, Position p2)
{
	c->size.width = abs(p1.x - p2.x);
	c->size.height = abs(p1.y - p2.y);
	c->size.x = p1.x;
	c->size.y = p1.y;
}

static bool
grab_pointer(Window w, Cursor cursor)
{
	const int gm=GrabModeAsync;
	return XGrabPointer(D, w, false, MouseMask, gm, gm, None, 
		cursor, CurrentTime) == GrabSuccess;
}

static void
handle_motion_notify(Client * c, XSizeHints * g, XMotionEvent * mev)
{
	Position p1={.x=g->x, .y=g->y};
	Position p2={.x=mev->x, .y=mev->y};
	draw_outline(c); // clear
	recalculate_sweep(c, p1, p2);
	draw_outline(c);
}

void
sweep(Client * c)
{
	/* Resizing shaded windows yields undefined behavior.  */
	if(!grab_pointer(c->screen->root, jbwm.X.cursor) 
		|| c->flags & JB_CLIENT_NO_RESIZE || c->flags & JB_CLIENT_SHADED) 
		return;
	XSizeHints *g=&(c->size);
	XWarpPointer(D, None, c->window, 0, 0, 0, 0, g->width-1, g->height-1);
	XEvent ev;
sweep_loop:	
	XMaskEvent(D, MouseMask, &ev);
	switch (ev.type)
	{
	case MotionNotify:
		handle_motion_notify(c, g, &(ev.xmotion));
		break;
	case ButtonRelease:
		XUngrabPointer(D, CurrentTime);
		moveresize(c);
		return;
	}
	goto sweep_loop;
}

#ifdef USE_SNAP

static inline void sborder(int *xy, const int border)
{
	if(abs(*xy+border)<JBWM_SNAP)
		*xy=-border;	
}

static void
snap_client_to_screen_border(Client * c)
{
	XSizeHints *g = &(c->size);
	/* snap to screen border */
	const ubyte b = c->border;
	sborder(&g->x, -b);
	sborder(&g->x, g->width + 2*b - c->screen->width);
#ifdef USE_TBAR
	sborder(&g->y, -b-((c->flags&JB_CLIENT_NO_TB)?0:TDIM));
#else//!USE_TBAR
	sborder(&g->y, -b);
#endif//USE_TBAR
	sborder(&g->y, g->height + 2*b - c->screen->height);
}

__attribute__((const))
static inline int 
absmin(const int a, const int b)
{
	return abs(a)<abs(b)?a:b;
}

static int
snap_dim(int *cxy, int *cwh, int *cixy, int *ciwh, 
	const ubyte border, const ubyte snap)
{
	int d = snap;

	d = absmin(d, *cixy + *ciwh - *cxy + 2 * border);
	d = absmin(d, *cixy + *ciwh - *cxy - *cwh);
	d = absmin(d, *cixy - *cxy - *cwh - 2 * border);
	d = absmin(d, *cixy - *cxy);

	return d;
}

static void
snap_client(Client * c)
{
#define S JBWM_SNAP
	/* snap to other windows */
	snap_client_to_screen_border(c);
	XSizeHints *g = &(c->size);
	Position d = {S, S};
	for(Client *ci = jbwm.head; ci; ci = ci->next)
	{
		XSizeHints *gi = &(ci->size);

		if(ci != c && (ci->screen == c->screen)
			&& (ci->vdesk == c->vdesk))
		{
			const ubyte border = c->border;

			if((gi->y - g->height - g->y <= S)
				&& (g->y - gi->height - gi->y <= S))
				d.x = snap_dim(&(g->x), &(g->width),
					&(gi->x), &(gi->width),
					border, S);
			if((gi->x - g->width - g->x <= S)
				&& (g->x - gi->width - gi->x <= S))
				d.y = snap_dim(&(g->y), &(g->height),
					&(gi->y), &(gi->height),
					border, S);
		}
	}
	if(abs(d.x) < S)
		g->x += d.x;
	if(abs(d.y) < S)
		g->y += d.y;
}
#endif /* USE_SNAP */

static inline void
drag_motion(Client * c, XEvent ev, int x1, int y1, int old_cx,
	int old_cy)
{
	XSizeHints *g=&(c->size);
	g->x = old_cx + (ev.xmotion.x - x1);
	g->y = old_cy + (ev.xmotion.y - y1);
#ifdef USE_SNAP
	snap_client(c);
#endif//USE_SNAP
	XMoveWindow(D, c->parent, g->x, g->y
#ifdef USE_TBAR
		-(!(c->flags&JB_CLIENT_NO_TB)?TDIM:0)
#endif//USE_TBAR
		);
}

static void
drag_event_loop(Client * c, int x1, int y1, int old_cx, int old_cy)
{
	XEvent ev;
drag_loop:
	XMaskEvent(D, MouseMask, &ev);
#ifdef USE_TBAR
	const bool no_tb=c->flags&JB_CLIENT_NO_TB;
#endif//USE_TBAR
	if(ev.type == MotionNotify)
	{
#ifdef USE_TBAR
		const int ny=y1+(no_tb?TDIM:0);
#else//!USE_TBAR
		const int ny=y1;
#endif//USE_TBAR
		drag_motion(c, ev, x1, ny, old_cx, old_cy);
	}
	else
	{
		XUngrabPointer(D, CurrentTime);
#ifdef USE_TBAR
		if(no_tb)
			c->size.y+=TDIM;
#endif//USE_TBAR
		moveresize(c);
		return;
	}
	goto drag_loop;
}

void
get_mouse_position(Window w, int *x, int *y)
{
	/* Dummy variables to satisfy required arguments.  */
	int __attribute__((unused)) d;
	unsigned int __attribute__((unused)) u;

	XQueryPointer(D, w, &w, &w, x, y, &d, &d, &u);
}

void
drag(Client * c)
{
	if((c->flags & JB_CLIENT_NO_RESIZE) && !(c->flags & JB_CLIENT_TEAROFF))
		return;
	const Window root=c->screen->root;
	if(!grab_pointer(root, jbwm.X.cursor))
		return;
	XSizeHints *g=&(c->size);
	Position old_p;
	old_p.x = g->x;
	old_p.y = g->y;
	Position p;
	get_mouse_position(root, &(p.x), &(p.y));
	drag_event_loop(c, p.x, p.y, old_p.x, old_p.y);
}

void
moveresize(Client * c)
{
	XSizeHints *g = &(c->size);
	const ubyte b = c->border;
#ifdef USE_TBAR
	const bool max = c->flags & JB_CLIENT_MAXIMIZED;
	const unsigned int parent_height = g->height + (max?0:TDIM);
	const short y = g->y-b-(max?0:TDIM);
#else//!USE_TBAR
	const unsigned int parent_height = g->height;
	const short y = g->y-b;
#endif//USE_TBAR
	const unsigned short width = g->width;
	const short x = g->x-b;

	if(c->flags & JB_CLIENT_SHADED)
	{
		XMoveWindow(D, c->parent, x, y);
		return;
	}
	/* Offset the child window within the parent window
	 to display titlebar */
	bool no_offset=(c->flags&JB_CLIENT_MAXIMIZED)
		||(c->flags&JB_CLIENT_NO_TB);
	/* The modifier to width enables correct display of the right-hand
	 edge of shaped windows.  */
	XMoveResizeWindow(D, c->parent, x, y, width+(no_offset?1:0), 
		parent_height);
#ifndef USE_TBAR
	no_offset=true;
#endif//USE_TBAR
	XMoveResizeWindow(D, c->window, 0, no_offset ? 0 : TDIM, 
		width, g->height);
	configure(c);
#ifdef USE_TBAR
	/* Only update the titlebar if the width has changed.  */
	if(g->width != c->exposed_width)
		update_titlebar(c);
	/* Store width value for above test.  */
	c->exposed_width = width;
#endif//USE_TBAR
#ifdef USE_SHAPE
	set_shape(c);
#endif//USE_SHAPE
}

void
maximize(Client * c)
{
	LOG("maximize()");
	// Honor !MWM_FUNC_MAXIMIZE
	if(c->flags & JB_CLIENT_NO_MAX)
		return;
	XSizeHints *g = &(c->size);
	if(c->flags & JB_CLIENT_MAXIMIZED) // restore:
	{
		memcpy(g, &(c->old_size), sizeof(XSizeHints));
		c->flags &= ~ JB_CLIENT_MAXIMIZED;
#ifdef EWMH
		ewmh_remove_state(c->window, ewmh.WM_STATE_FULLSCREEN);
		ewmh_remove_state(c->window, ewmh.WM_STATE_MAXIMIZED_HORZ);
		ewmh_remove_state(c->window, ewmh.WM_STATE_MAXIMIZED_VERT);
#endif//EWMH
	}
	else // maximize:
	{
		memcpy(&(c->old_size), g, sizeof(XSizeHints));
		g->x = g->y = 0;
		ScreenInfo *s = c->screen;
		g->width = s->width;
		g->height = s->height;
		c->flags |= JB_CLIENT_MAXIMIZED;
#ifdef EWMH
		ewmh_add_state(c->window, ewmh.WM_STATE_FULLSCREEN);
		ewmh_add_state(c->window, ewmh.WM_STATE_MAXIMIZED_HORZ);
		ewmh_add_state(c->window, ewmh.WM_STATE_MAXIMIZED_VERT);
#endif//EWMH
	}
	moveresize(c);
	XRaiseWindow(D, c->parent);
}

void
hide(Client * c)
{
	/* This will generate an unmap event.  Tell event handler
	 * to ignore it.  */
	c->ignore_unmap++;
	XUnmapWindow(D, c->parent);
	set_wm_state(c, IconicState);
}

void
unhide(Client * c)
{
	XMapWindow(D, c->parent);
	set_wm_state(c, NormalState);
}

ubyte
switch_vdesk(ScreenInfo * s, const ubyte v)
{
	if(v==s->vdesk || v>DESKTOPS) return s->vdesk;
	for(Client *c=jbwm.head; c; c=c->next)
	{
		if(is_sticky(c))
		{
			unhide(c);
			continue;
		}
		if(c->screen!=s)
			continue;
		if(c->vdesk == s->vdesk)
			hide(c);
		else if(c->vdesk == v)
			unhide(c);
	}
	s->vdesk=v;
#ifdef EWMH
	XPROP(s->root, ewmh.CURRENT_DESKTOP, XA_CARDINAL, &(s->vdesk), 1);
#endif//EWMH
	return s->vdesk;
}

