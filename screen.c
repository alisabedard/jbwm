/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2015, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */
#include "jbwm.h"
#include <string.h>
#include "screen.h"

#define MouseMask (ButtonPressMask|ButtonReleaseMask|PointerMotionMask)

static inline void
draw_outline(Client * c)
{
#ifdef USE_SHAPE
	if(is_shaped(c))
		return;
#endif /* USE_SHAPE */
	XRectangle *g=&(c->geometry);
	ScreenInfo *s=c->screen;
	XDrawRectangle(D, s->root, s->gc, g->x, g->y-TDIM, g->width, 
		g->height + (c->flags & JB_CLIENT_SHADED ?0:TDIM));
}

static inline void
recalculate_size(Client * c, Position p1, Position p2)
{
	c->geometry.width = abs(p1.x - p2.x);
	c->geometry.height = abs(p1.y - p2.y);
}

static inline void
recalculate_sweep(Client * c, Position p1, Position p2)
{
	recalculate_size(c, p1, p2);
	c->geometry.x = p1.x;
	c->geometry.y = p1.y;
	configure(c);
}

static bool
grab_pointer(Window w, Cursor cursor)
{
	const int gm=GrabModeAsync;
	return XGrabPointer(D, w, false, MouseMask, gm, gm, None, 
		cursor, CurrentTime) == GrabSuccess;
}

static void
handle_motion_notify(Client * c, XRectangle * g, XMotionEvent * mev)
{
	draw_outline(c);
	Position p1;
	p1.x = g->x;
	p1.y = g->y;
	Position p2;
	p2.x = mev->x;
	p2.y = mev->y;
	recalculate_sweep(c, p1, p2);
	draw_outline(c);
}

void
sweep(Client * c)
{

	/* Resizing shaded windows yields undefined behavior.  */
	if(!grab_pointer(c->screen->root, jbwm.X.cursor) 
		|| c->flags & JB_CLIENT_SHADED) return;
	XRectangle *g=&(c->geometry);
	XWarpPointer(D, None, c->window, 0, 0, 0, 0, 
		g->width, g->height);
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

static inline void sborder(short *xy, const ubyte border)
{
	if(abs(*xy+border)<JBWM_SNAP)
		*xy=-border;	
}

static void
snap_client_to_screen_border(Client * c)
{
	XRectangle *g = &(c->geometry);
	/* snap to screen border */
	const ubyte b = c->border;
	sborder(&g->x, -b);
	sborder(&g->x, g->width + b - c->screen->width);
	sborder(&g->y, -b);
	sborder(&g->y, g->height + b - c->screen->height);
}

static inline int 
absmin(const int a, const int b)
{
	return abs(a)<abs(b)?a:b;
}

static int
snap_dim(short *cxy, unsigned short *cwh, short *cixy,
	unsigned short *ciwh, const ubyte border, const ubyte snap)
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
	int dx, dy;
#define S JBWM_SNAP
	/* snap to other windows */
	snap_client_to_screen_border(c);
	dx = dy = S;
	XRectangle *g = &(c->geometry);
	for(Client *ci = head_client; ci; ci = ci->next)
	{
		XRectangle *gi = &(ci->geometry);

		if(ci != c && (ci->screen == c->screen)
			&& (ci->vdesk == c->vdesk))
		{
			const ubyte border = c->border;

			if(gi->y - g->height - g->y <= S
				&& g->y - gi->height - gi->y <= S)
				dx = snap_dim(&(g->x), &(g->width),
					&(gi->x), &(gi->width),
					border, S);
			if(gi->x - g->width - g->x <= S
				&& g->x - gi->width - gi->x <= S)
				dy = snap_dim(&(g->y), &(g->height),
					&(gi->y), &(gi->height),
					border, S);
		}
	}
	if(abs(dx) < S)
		g->x += dx;
	if(abs(dy) < S)
		g->y += dy;
}
#endif /* USE_SNAP */

static inline void
drag_motion(Client * c, XEvent ev, int x1, int y1, int old_cx,
	int old_cy)
{
	draw_outline(c);	/* clear */
	XRectangle *g=&(c->geometry);
	g->x = old_cx + (ev.xmotion.x - x1);
	g->y = old_cy + (ev.xmotion.y - y1);
	configure(c);
#ifdef USE_SNAP
	snap_client(c);
#endif /* USE_SNAP */
	draw_outline(c);
}

static void
drag_event_loop(Client * c, int x1, int y1, int old_cx, int old_cy)
{
	XEvent ev;
drag_loop:
	XMaskEvent(D, MouseMask, &ev);
	if(ev.type == MotionNotify)
		drag_motion(c, ev, x1, y1, old_cx, old_cy);
	else
	{
		XUngrabPointer(D, CurrentTime);
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
	Window root=c->screen->root;
	Position p, old_p;
	XRectangle *g;
	if(!grab_pointer(root, jbwm.X.cursor))
		return;
	g=&(c->geometry);
	old_p.x = g->x;
	old_p.y = g->y;
	get_mouse_position(root, &(p.x), &(p.y));
	drag_event_loop(c, p.x, p.y, old_p.x, old_p.y);
}

void
moveresize(Client * c)
{
	XRectangle *g = &(c->geometry);
	const bool max = c->flags & JB_CLIENT_MAXIMIZED;
	const ubyte b = c->border;
#ifdef USE_TBAR
	const unsigned int parent_height = g->height + (max?0:TDIM);
	const short y = g->y-b-(max?0:TDIM);
#else
	const unsigned int parent_height = g->height;
	const short y = g->y-b;
#endif
	const unsigned short width = g->width;
	const short x = g->x-b;

	if(c->flags & JB_CLIENT_SHADED)
	{
		XMoveWindow(D, c->parent, x, y);
		return;
	}
		
	XMoveResizeWindow(D, c->parent, x, y, width, parent_height);
	/* Offset the child window within the parent window
		to display titlebar */
#ifdef USE_TBAR
	XMoveResizeWindow(D, c->window, 0, 
		c->flags & JB_CLIENT_MAXIMIZED ? 0 : TDIM, width, g->height);
#endif//USE_TBAR
	send_configure(c);
#ifdef USE_TBAR
	/* Only update the titlebar if the width has changed.  */
	if(g->width != c->exposed_width)
		update_titlebar(c);
#endif//USE_TBAR
	/* Store width value for above test.  */
	c->exposed_width = width;
}

void
maximize(Client * c)
{
	LOG("maximize()");
	XRectangle *g = &(c->geometry);
	if(c->flags & JB_CLIENT_MAXIMIZED) // restore:
	{
		memcpy(g, &(c->old_geometry), sizeof(XRectangle));
		c->flags &= ~ JB_CLIENT_MAXIMIZED;
#ifdef EWMH
		XDeleteProperty(D, c->window, XA("_NET_WM_STATE_FULLSCREEN"));
#endif//EWMH
	}
	else // maximize:
	{
		memcpy(&(c->old_geometry), g, sizeof(XRectangle));
		g->x = g->y = 0;
		ScreenInfo *s = c->screen;
		g->width = s->width;
		g->height = s->height;
		c->flags |= JB_CLIENT_MAXIMIZED;
#ifdef EWMH
		XPROP(c->window, ewmh.WM_STATE, XA_ATOM, 
			&ewmh.WM_STATE_FULLSCREEN, 1);
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

void
switch_vdesk(ScreenInfo * s, const ubyte v)
{
	if(v==s->vdesk) return;
	for(Client *c=head_client; c; c=c->next)
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
}

