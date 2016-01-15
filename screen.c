// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

#define MouseMask (ButtonPressMask|ButtonReleaseMask|PointerMotionMask)

__attribute__((hot))
static void
draw_outline(Client * c)
{
	assert(c);
	XSizeHints *g=&(c->size);
	XRectangle d={.x=g->x, .y=g->y, .width=g->width, .height=g->height};
	ScreenInfo *s=c->screen;
	XDrawRectangles(D, s->root, s->gc, &d, 1);
}

static void
configure(XSizeHints *g, const Window w)
{
	XConfigureEvent ce={.x=g->x, .y=g->y, .width=g->width,
		.height=g->height, .type=ConfigureNotify, .event=w};
	XSendEvent(D, w, true, StructureNotifyMask, (XEvent *)&ce);
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
}

static bool
grab_pointer(Window w, Cursor cursor)
{
	return XGrabPointer(D, w, false, MouseMask, GrabModeAsync, 
		GrabModeAsync, None, cursor, CurrentTime) == GrabSuccess;
}

static void
handle_motion_notify(Client * c, XSizeHints * g, XMotionEvent * mev)
{
	Position p1={.x=g->x, .y=g->y};
	Position p2={.x=mev->x, .y=mev->y};
	if(c->border)
		draw_outline(c); // clear outline
	recalculate_sweep(c, p1, p2);
	if(c->border)
		draw_outline(c);
	else
		moveresize(c);
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
	if(f&JB_SHAPED) // Bugfix for offset issue
		g->height+=TDIM;
#endif//USE_SHAPE
	XWarpPointer(D, None, c->window, 0, 0, 0, 0, g->width-1, g->height-1);
	XEvent ev;
	if(c->border)
		XGrabServer(D);
sweep_loop:	
	XMaskEvent(D, MouseMask, &ev);
	switch (ev.type)
	{
	case MotionNotify:
		handle_motion_notify(c, g, &(ev.xmotion));
		break;
	case ButtonRelease:
		configure(&(c->size), c->window);
		if(c->border)
			XUngrabServer(D);
		XUngrabPointer(D, CurrentTime);
		moveresize(c);
		return;
	}
	goto sweep_loop;
}

static void
drag_motion(Client * c, XEvent ev, const Position p, const Position oldp)
{
	XSizeHints *g=&(c->size);
	const bool outline=c->border && !(c->flags&JB_SHADED);
	if(outline)
		draw_outline(c);
	g->x = oldp.x + (ev.xmotion.x - p.x);
	g->y = oldp.y + (ev.xmotion.y - p.y);
#ifdef USE_SNAP
	snap_client(c);
#endif//USE_SNAP
	if(outline)
		draw_outline(c);
	else
		moveresize(c);
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
		configure(&(c->size), c->window);
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
moveresize(Client *restrict c)
{
	const bool no_tb=c->flags&JB_NO_TB||c->flags&JB_MAXIMIZED;
	const uint8_t offset=no_tb?0:TDIM;
	XMoveResizeWindow(D, c->parent, c->size.x, c->size.y-offset, 
		c->size.width, c->size.height+offset);
	XMoveResizeWindow(D, c->window, 0, offset, c->size.width, 
		c->size.height);
	if(!no_tb) update_titlebar(c);
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
#ifdef  USE_SHAPE
	// Currently buggy, so return.
	if(c->flags & JB_SHAPED)
		return;
#endif//USE_SHAPE
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

uint8_t
switch_vdesk(ScreenInfo * s, const uint8_t v)
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

