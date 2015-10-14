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
	const ubyte h=TITLEBAR_HEIGHT;
#ifdef USE_SHAPE
	if(is_shaped(c))
		return;
#endif /* USE_SHAPE */
#define CG c->geometry
	XDrawRectangle(jbwm.X.dpy, c->screen->root, c->screen->gc, 
		CG.x, CG.y-h, CG.width, CG.height 
		+ ((c->flags & JB_CLIENT_SHADED) ? 0 : h));
}

static void
recalculate_size(Client * c, Position p1, Position p2)
{
	XRectangle *g = &(CG);

	g->width = abs(p1.x - p2.x);
	g->height = abs(p1.y - p2.y);
}

static void
recalculate_sweep(Client * c, Position p1, Position p2)
{
	recalculate_size(c, p1, p2);
	CG.x = p1.x;
	CG.y = p1.y;

	SET_CLIENT_CE(c);
}

#define grab_pointer(w, mask, curs) \
	(XGrabPointer(jbwm.X.dpy, w, false, mask, GrabModeAsync,\
	GrabModeAsync, None, curs, CurrentTime) == GrabSuccess)

static void
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
	if(c->flags & JB_CLIENT_SHADED)
		return;

	g = &(c->geometry);

	if(!grab_pointer(c->screen->root, MouseMask, jbwm.X.cursor))
		return;
	setmouse(c->window, g->width, g->height);
	for(;;)
	{
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
	}
}

#ifdef USE_SNAP
static void
snap_client_to_screen_border(Client * c)
{
	XRectangle *g;
	const unsigned short dw = c->screen->width;
	const unsigned short dh = c->screen->height;
	const ubyte snap = JBWM_SNAP_DISTANCE;
	const ubyte b = c->border;

	g = &(c->geometry);
	/* snap to screen border */
#define SBORDER(xy, b) if(abs(g->xy+b)<snap) g->xy=-(b);
	SBORDER(x, -b);
	SBORDER(x, g->width + b - dw);
	SBORDER(y, -b);
	SBORDER(y, g->height + b - dh);
}

#define absmin(a, b) (abs(a)<abs(b)?a:b)
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
	const ubyte snap = JBWM_SNAP_DISTANCE;

	snap_client_to_screen_border(c);
	/* snap to other windows */
	dx = dy = snap;
	for(ci = head_client; ci; ci = ci->next)
	{
		XRectangle *gi = &(ci->geometry);

		if(ci != c && (ci->screen == c->screen)
			&& (ci->vdesk == c->vdesk))
		{
			const ubyte border = c->border;

			if(gi->y - g->height - g->y <= snap
				&& g->y - gi->height - gi->y <= snap)
				dx = snap_dim(&(g->x), &(g->width),
					&(gi->x), &(gi->width),
					border, snap);
			if(gi->x - g->width - g->x <= snap
				&& g->x - gi->width - gi->x <= snap)
				dy = snap_dim(&(g->y), &(g->height),
					&(gi->y), &(gi->height),
					border, snap);
		}
	}
	if(abs(dx) < snap)
		g->x += dx;
	if(abs(dy) < snap)
		g->y += dy;
}
#endif /* USE_SNAP */

static inline void
drag_motion(Client * c, XEvent ev, int x1, int y1, int old_cx,
	int old_cy)
{
	draw_outline(c);	/* clear */
	c->geometry.x = old_cx + (ev.xmotion.x - x1);
	c->geometry.y = old_cy + (ev.xmotion.y - y1);
	SET_CLIENT_CE(c);
#ifdef USE_SNAP
	snap_client(c);
#endif /* USE_SNAP */
	draw_outline(c);
}

static void
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
drag(Client * c)
{
	Position p, old_p;
	Window root;

	root = c->screen->root;
	if(!grab_pointer(root, MouseMask, jbwm.X.cursor))
		return;
	old_p.x = c->geometry.x;
	old_p.y = c->geometry.y;
	get_mouse_position((int *)&(p.x), (int *)&(p.y), root);
	drag_event_loop(c, p.x, p.y, old_p.x, old_p.y);
}

void
moveresize(Client * c)
{
#define TB TITLEBAR_HEIGHT
	XRectangle *g = &(c->geometry);
	const unsigned int parent_height = g->height + TB;
	const ubyte b = c->border;
	const unsigned short width = g->width;

	XMoveResizeWindow(jbwm.X.dpy, c->parent, g->x - b, g->y - b - TB, 
		width, parent_height);
	/* Offset the child window within the parent window
		to display titlebar */
	XMoveResizeWindow(jbwm.X.dpy, c->window, 0, TB, width, 
		g->height + TB);
	send_config(c);
#ifdef USE_TBAR
	/* Only update the titlebar if the width has changed.  */
	if(g->width != c->exposed_width)
		update_info_window(c);
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
		g->y=TITLEBAR_HEIGHT;
		g->width = s->width;
		g->height = s->height-TITLEBAR_HEIGHT;
		c->flags |= JB_CLIENT_MAXIMIZED;
#if 0
		g->x = g->y = 0;
		g->width = s->width;
		g->height = s->height;
#endif
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
next(void)
{
	Client *newc = current;

	do
	{
		if(newc)
		{
			newc = newc->next;
			if(!newc && !current)
				return;
		}
		else
			newc = head_client;
		if(!newc || newc == current)
			return;
	}
	while(newc->vdesk != newc->screen->vdesk);
	/*
	 * NOTE: Checking against newc->screen->vdesk implies we can Alt+Tab
	 * across screen boundaries.  Is this what we want?
	 */

	unhide(newc);
	select_client(newc);
	{
		XRectangle *g = &(newc->geometry);
		setmouse(newc->window, g->width, g->height);
	}
}

void
switch_vdesk(ScreenInfo * s, const ubyte v)
{
	Client *c;
	
	if(v==s->vdesk)
		return;
	if(current && !is_sticky(current))
		select_client(NULL);
	for(c=head_client; c; c=c->next)
	{
		if(c->screen!=s)
			continue;
		if(c->vdesk == s->vdesk)
			hide(c);
		else if(c->vdesk == v)
			unhide(c);
			
	}
	s->vdesk=v;
}
