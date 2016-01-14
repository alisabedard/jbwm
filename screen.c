// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

#define MouseMask (ButtonPressMask|ButtonReleaseMask|PointerMotionMask)

#ifdef USE_SNAP
static void
snap_client(Client * c);
#endif//USE_SNAP

__attribute__((hot))
static void
draw_outline(Client * c)
{
	assert(c);
	if((c->flags & JB_NO_TB) || !c->border)
	{
		moveresize(c);
		return;
	}
	ScreenInfo *s=c->screen;
	const Window r=s->root;
	XSizeHints *g=&(c->size);
	XRectangle d={.x=g->x, .y=g->y, .width=g->width, .height=g->height};
#ifdef USE_TBAR
	d.y-=TDIM;
	d.height+=TDIM;
#endif//USE_TBAR
	XDrawRectangles(D, r, s->gc, &d, 1);
}

static void
recalculate_sweep(Client * c, Position p1, Position p2)
{
	LOG("recalculate_sweep");
	assert(c);
	c->size.width = abs(p1.x - p2.x);
	c->size.height = abs(p1.y - p2.y);
	c->size.x = p1.x;
	c->size.y = p1.y;
	XConfigureEvent ce={.type=ConfigureNotify, .event=c->window, .x=p1.x, 
		.y=p1.y, .width=c->size.width, .height=c->size.height};
	XSendEvent(D, c->window, true, StructureNotifyMask, (XEvent *)&ce);
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
#ifdef USE_SHAPE
	const bool s=(c->flags&JB_SHAPED);
	if(!s)
		draw_outline(c); //clear
	recalculate_sweep(c, p1, p2);
	if(!s)
		draw_outline(c);
	else
		moveresize(c);
#else//!USE_SHAPE
	draw_outline(c);
	recalculate_sweep(c, p1, p2);
	draw_outline(c);
#endif//USE_SHAPE
}

void
sweep(Client * c)
{
	LOG("sweep");
	assert(c);
	/* Resizing shaded windows yields undefined behavior.  */
	const uint32_t f=c->flags;
	if(f&JB_NO_RESIZE||f&JB_SHADED)
		return;
	if(!grab_pointer(c->screen->root, jbwm.X.cursor))
		return;
	XSizeHints *g=&(c->size);
#ifdef USE_SHAPE
	const bool shaped=f&JB_SHAPED;
	if(shaped) // Bugfix for offset issue
		g->height+=TDIM;
#endif//USE_SHAPE
	XWarpPointer(D, None, c->window, 0, 0, 0, 0, g->width-1, g->height-1);
	XEvent ev;
#ifdef USE_SHAPE
	if(shaped)
		goto sweep_loop;
#endif//USE_SHAPE
	XGrabServer(D);
sweep_loop:	
	XMaskEvent(D, MouseMask, &ev);
	switch (ev.type)
	{
	case MotionNotify:
		handle_motion_notify(c, g, &(ev.xmotion));
		break;
	case ButtonRelease:
#ifdef USE_SHAPE
		if(!shaped)
#endif//USE_SHAPE
		XUngrabServer(D);
		XUngrabPointer(D, CurrentTime);
		moveresize(c);
		return;
	}
	goto sweep_loop;
}

#ifdef USE_SNAP

//__attribute__((const,hot))
__attribute__((hot))
static void
sborder(int *xy, const int border)
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
	sborder(&g->x, g->width - c->screen->width + 2*b);
	sborder(&g->y, -b-((c->flags&JB_NO_TB)?0:TDIM));
	sborder(&g->y, g->height + 2*b - c->screen->height);
}

__attribute__((const,hot))
static int 
absmin(const int a, const int b)
{
	return abs(a)<abs(b)?a:b;
}

static int
snap_dim(int *cxy, int *cwh, int *cixy, int *ciwh, 
	const ubyte border, int d)
{
	int s=*cixy+*ciwh;
	d = absmin(d, s - *cxy+border);
	d = absmin(d, s - *cxy - *cwh);
	s=*cixy-*cxy;
	d = absmin(d, s - *cwh-border);
	d = absmin(d, s);

	return d;
}

static void
snap_client(Client * c)
{
	XLOG("snap_client");
	assert(c);
	snap_client_to_screen_border(c);
	// Snap to other windows:
	XSizeHints *g = &(c->size);
	const ubyte s=JBWM_SNAP;
	Position d = {s, s};
	for(Client *ci = jbwm.head; ci; ci = ci->next)
	{
		if((ci==c)||(ci->screen!=c->screen)||(ci->vdesk!=c->vdesk))
			continue;
		XSizeHints *gi = &(ci->size);
		const ubyte b = c->border;
		if((gi->y - g->height - g->y <= s) 
			&& (g->y - gi->height - gi->y <= s))
		{
			d.x = snap_dim(&(g->x), &(g->width), &(gi->x), 
				&(gi->width), b, s);
		}
		if((gi->x - g->width - g->x <= s) 
			&& (g->x - gi->width - gi->x <= s))
		{
			d.y = snap_dim(&(g->y), &(g->height), &(gi->y), 
				&(gi->height), b, s);
		}
	}
	if(abs(d.x) < s)
		g->x += d.x;
	if(abs(d.y) < s)
		g->y += d.y;
}
#endif /* USE_SNAP */

static void
drag_motion(Client * c, XEvent ev, const Position p, const Position oldp)
{
	XSizeHints *g=&(c->size);
	g->x = oldp.x + (ev.xmotion.x - p.x);
	g->y = oldp.y + (ev.xmotion.y - p.y);
#ifdef USE_SNAP
	snap_client(c);
#endif//USE_SNAP
	XMoveWindow(D, c->parent, g->x, g->y
#ifdef USE_TBAR
		-(!(c->flags&JB_NO_TB)?TDIM:0)
#endif//USE_TBAR
		);
	XConfigureEvent ce={.type=ConfigureNotify, .event=c->window, 
		.x=g->x, .y=g->y, .width=g->width, .height=g->height};
	XSendEvent(D, c->window, true, StructureNotifyMask, (XEvent *)&ce);
}

static void
drag_event_loop(Client * c, Position p, const Position oldp)
{
	XEvent ev;
#ifdef USE_TBAR
	p.y+=c->flags&JB_NO_TB?TDIM:0;
#endif//USE_TBAR
	XGrabServer(D);
drag_loop:
	XMaskEvent(D, MouseMask, &ev);
	if(ev.type == MotionNotify)
		drag_motion(c, ev, p, oldp);
	else
	{
		XUngrabPointer(D, CurrentTime);
		moveresize(c);
		XUngrabServer(D);
		return;
	}
	goto drag_loop;
}

void
get_mouse_position(Window w, Position *p)
{
	XQueryPointer(D, w, &w, &w, &p->x, &p->y, 
		(int*)&w, (int*)&w, (unsigned int*)&w);
}

void
drag(Client * c)
{
	LOG("drag");
	assert(c);
	if(c->flags & JB_NO_MOVE)
		return;
	const Window root=c->screen->root;
	if(!grab_pointer(root, jbwm.X.cursor))
		return;
	XSizeHints *g=&(c->size);
	Position oldp={.x=g->x, .y=g->y};
	Position p;
	get_mouse_position(root, &p);
	drag_event_loop(c, p, oldp);
}

void
moveresize(Client * c)
{
	LOG("moveresize");
	assert(c);
	XSizeHints *g = &(c->size);
#ifdef USE_TBAR
	const uint32_t f=c->flags;
	bool no_offset=f&JB_MAXIMIZED||f&JB_NO_TB;
#ifdef USE_SHAPE
	no_offset|=f&JB_SHAPED;
#endif//USE_SHAPE
	const ubyte offset=no_offset?0:TDIM;
#ifdef USE_SNAP
#ifdef USE_SHAPE
	if(!(f&JB_SHAPED)) // Buggy if shaped
#endif//USE_SHAPE
		snap_client_to_screen_border(c);
#endif//USE_SNAP
#else//!USE_TBAR
#define offset 0
#endif//USE_TBAR
	const ubyte b=c->border;
	XRectangle d={.x=g->x-b, .y=g->y-b-offset, .width=g->width, 
		.height=g->height+offset};
	XMoveResizeWindow(D, c->parent, d.x, d.y, d.width, d.height);
	/* The modifier to width enables correct display of the right-hand
	 edge of shaped windows.  */
	XMoveResizeWindow(D, c->window, 0, offset, d.width, g->height);
#ifdef USE_TBAR
	/* Only update the titlebar if the width has changed or if the
	 titlebar must be removed */
	if(no_offset || (d.width != c->exposed_width))
		update_titlebar(c);
	/* Store width value for above test.  */
	c->exposed_width = d.width;
#endif//USE_TBAR
#ifdef USE_SHAPE
	set_shape(c);
#endif//USE_SHAPE
}

void
maximize(Client * c)
{
	LOG("maximize");
	assert(c);
	// Honor !MWM_FUNC_MAXIMIZE
	if(c->flags & JB_NO_MAX)
		return;
#if 0
#ifdef  USE_SHAPE
	// Currently buggy, so return.
	if(c->flags & JB_SHAPED)
		return;
#endif//USE_SHAPE
#endif
	XSizeHints *g = &(c->size);
	if(c->flags & JB_MAXIMIZED) // restore:
	{
		memcpy(g, &(c->old_size), sizeof(XSizeHints));
		c->flags &= ~ JB_MAXIMIZED;
#ifdef EWMH
		ewmh_remove_state(c->window, ewmh.WM_STATE_FULLSCREEN);
		ewmh_remove_state(c->window, ewmh.WM_STATE_MAXIMIZED_HORZ);
		ewmh_remove_state(c->window, ewmh.WM_STATE_MAXIMIZED_VERT);
#endif//EWMH
		XSetWindowBorderWidth(D, c->parent, c->border);
	}
	else // maximize:
	{
		memcpy(&(c->old_size), g, sizeof(XSizeHints));
		g->x = g->y = 0;
		ScreenInfo *s = c->screen;
		g->width = s->width;
		g->height = s->height;
		c->flags |= JB_MAXIMIZED;
#ifdef EWMH
		ewmh_add_state(c->window, ewmh.WM_STATE_FULLSCREEN);
		ewmh_add_state(c->window, ewmh.WM_STATE_MAXIMIZED_HORZ);
		ewmh_add_state(c->window, ewmh.WM_STATE_MAXIMIZED_VERT);
#endif//EWMH
		XSetWindowBorderWidth(D, c->parent, 0);
	}
	moveresize(c);
	XRaiseWindow(D, c->parent);
}

void
hide(Client * c)
{
	LOG("hide");
	assert(c);
	/* This will generate an unmap event.  Tell event handler
	 * to ignore it.  */
	c->ignore_unmap++;
	XUnmapWindow(D, c->parent);
	set_wm_state(c, IconicState);
}

void
unhide(Client * c)
{
	LOG("unhide");
	assert(c);
	XMapWindow(D, c->parent);
	set_wm_state(c, NormalState);
}

ubyte
switch_vdesk(ScreenInfo * s, const ubyte v)
{
	LOG("switch_vdesk");
	assert(s);
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

