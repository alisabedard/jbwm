/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2015, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */
#include "jbwm.h"
#include <string.h>
#include "screen.h"

static inline void
draw_outline(Client * c)
{
	const bool shaded=c->flags & JB_CLIENT_SHADED; 
	XRectangle *g;
	ScreenInfo *scr;

#ifdef USE_SHAPE
	if(is_shaped(c))
		return;
#endif /* USE_SHAPE */
	g=&(c->geometry);
	scr=c->screen;
	XDrawRectangle(jbwm.X.dpy, scr->root, scr->gc, g->x, g->y-TDIM, 
		g->width, g->height + (shaded?0:TDIM));
}

static inline void
recalculate_size(Client * c, Position p1, Position p2)
{
	XRectangle *g = &(c->geometry);

	g->width = abs(p1.x - p2.x);
	g->height = abs(p1.y - p2.y);
}

static inline void
recalculate_sweep(Client * c, Position p1, Position p2)
{
	recalculate_size(c, p1, p2);
	c->geometry.x = p1.x;
	c->geometry.y = p1.y;
	configure(c);
}

static inline bool
grab_pointer(Window w, Cursor cursor)
{
	const int gm=GrabModeAsync;
	return XGrabPointer(jbwm.X.dpy, w, false, MouseMask, gm, gm, None, 
		cursor, CurrentTime) == GrabSuccess;
}

static inline void
handle_motion_notify(Client * c, XRectangle * g, XMotionEvent * mev)
{
	Position p1, p2;

	p1.x = g->x;
	p1.y = g->y;
	p2.x = mev->x;
	p2.y = mev->y;
	draw_outline(c);
	recalculate_sweep(c, p1, p2);
	draw_outline(c);
}

void
sweep(Client * c)
{
	XEvent ev;
	XRectangle *g;

	/* Resizing shaded windows yields undefined behavior.  */
	if(!grab_pointer(c->screen->root, jbwm.X.cursor) 
		|| c->flags & JB_CLIENT_SHADED)
	{
		return;
	}
	g = &(c->geometry);
	XWarpPointer(jbwm.X.dpy, None, c->window, 0, 0, 0, 0, 
		g->width, g->height);
sweep_loop:	
	XMaskEvent(jbwm.X.dpy, MouseMask, &ev);
	switch (ev.type)
	{
	case MotionNotify:
		handle_motion_notify(c, g, &(ev.xmotion));
		break;
	case ButtonRelease:
		XUngrabPointer(jbwm.X.dpy, CurrentTime);
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
	XRectangle *g;
	const unsigned short dw = c->screen->width;
	const unsigned short dh = c->screen->height;
	const ubyte b = c->border;

	g = &(c->geometry);
	/* snap to screen border */
	sborder(&g->x, -b);
	sborder(&g->x, g->width + b - dw);
	sborder(&g->y, -b);
	sborder(&g->y, g->height + b - dh);
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
	Client *ci;
	XRectangle *g = &(c->geometry);
#define S JBWM_SNAP

	snap_client_to_screen_border(c);
	/* snap to other windows */
	dx = dy = S;
	for(ci = head_client; ci; ci = ci->next)
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
	XRectangle *g;

	draw_outline(c);	/* clear */
	g=&(c->geometry);
	g->x = old_cx + (ev.xmotion.x - x1);
	g->y = old_cy + (ev.xmotion.y - y1);
	configure(c);
#ifdef USE_SNAP
	snap_client(c);
#endif /* USE_SNAP */
	draw_outline(c);
}

static inline void
drag_button_release(Client * c)
{
	XUngrabPointer(jbwm.X.dpy, CurrentTime);
	moveresize(c);
}

static void
drag_event_loop(Client * c, int x1, int y1, int old_cx, int old_cy)
{
	XEvent ev;

	for(;;)
	{
		XMaskEvent(jbwm.X.dpy, MouseMask, &ev);
		if(ev.type == MotionNotify)
			drag_motion(c, ev, x1, y1, old_cx, old_cy);
		else
		{
			drag_button_release(c);
			return;
		}
	}

}

void
get_mouse_position(Window w, int *x, int *y)
{
	/* Dummy variables to satisfy required arguments.  */
	int __attribute__((unused)) d;
	unsigned int __attribute__((unused)) u;

	XQueryPointer(jbwm.X.dpy, w, &w, &w, x, y, &d, &d, &u);
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
	const ubyte b = c->border;
#ifdef USE_TBAR
	const unsigned int parent_height = g->height + TDIM;
	const short y = g->y-b-TDIM;
#else
	const unsigned int parent_height = g->height;
	const short y = g->y-b;
#endif
	const unsigned short width = g->width;
	const short x = g->x-b;

	if(c->flags & JB_CLIENT_SHADED)
	{
		XMoveWindow(jbwm.X.dpy, c->parent, x, y);
		return;
	}
		
	XMoveResizeWindow(jbwm.X.dpy, c->parent, x, y, width, parent_height);
	/* Offset the child window within the parent window
		to display titlebar */
#ifdef USE_TBAR
	XMoveResizeWindow(jbwm.X.dpy, c->window, 0, TDIM, width, g->height);
#endif
	send_configure(c);
#ifdef USE_TBAR
	/* Only update the titlebar if the width has changed.  */
	if(g->width != c->exposed_width)
		update_titlebar(c);
#endif
	/* Store width value for above test.  */
	c->exposed_width = width;
}

void
maximize(Client * c)
{
	XRectangle *g;
	XRectangle *og;

	g = &(c->geometry);
	og = &(c->old_geometry);
	if(c->flags & JB_CLIENT_MAXIMIZED)
	{
		memcpy(g, og, sizeof(XRectangle));
		/* og->width is used as a flag here.  */
		c->flags &= ~ JB_CLIENT_MAXIMIZED;
		og->width = 0;
	}
	else
	{
		ScreenInfo *s = c->screen;

		memcpy(og, g, sizeof(XRectangle));
		g->x = 0;
		g->y=TDIM;
		g->width = s->width;
		g->height = s->height-TDIM;
		c->flags |= JB_CLIENT_MAXIMIZED;
	}
	moveresize(c);
	XRaiseWindow(jbwm.X.dpy, c->parent);
}

void
hide(Client * c)
{
	/* This will generate an unmap event.  Tell event handler
	 * to ignore it.  */
	c->ignore_unmap++;
	XUnmapWindow(jbwm.X.dpy, c->parent);
	set_wm_state(c, IconicState);
}

void
unhide(Client * c)
{
	XMapWindow(jbwm.X.dpy, c->parent);
	set_wm_state(c, NormalState);
}

void
switch_vdesk(ScreenInfo * s, const ubyte v)
{
	Client *c;
	
	if(v==s->vdesk)
		return;
	for(c=head_client; c; c=c->next)
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
}

