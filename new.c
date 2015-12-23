/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include <stdlib.h>
#include <string.h>
#include "jbwm.h"

static void init_geometry(Client * c);
static void reparent(Client * c);

static inline void
initialize_client_fields(Client * c, ScreenInfo * s, Window w)
{
	c->next = head_client;
	head_client = c;
	c->screen = s;
	c->window = w;
	c->window_type = None;
}

Client *
Client_new(Window w, ScreenInfo * s)
{
	Client *c = calloc(1, sizeof(Client));
	initialize_client_fields(c, s, w);
	c->border = JBWM_BORDER;
	init_geometry(c);
	return c;
}

void
make_new_client(Window w, ScreenInfo * s)
{
	Client *c=Client_new(w, s);
	if(c->flags & JB_CLIENT_DONT_MANAGE)
		return;
	XSelectInput(D, c->window, EnterWindowMask 
		| PropertyChangeMask | ColormapChangeMask);
#ifdef USE_SHAPE
	set_shape(c);
#endif /* USE_SHAPE */
	reparent(c);
	unhide(c);
	jbwm_grab_button(w, jbwm.keymasks.grab, AnyButton);
}

#ifdef EWMH
static void *
jbwm_get_property(Window w, Atom property, Atom req_type,
	unsigned long *nitems_return)
{
	Atom actual_type;
	int actual_format;
	unsigned long bytes_after;
	unsigned char *prop;

	if(XGetWindowProperty(D, w, property, 0L, 1024, false, 
		req_type, &actual_type, &actual_format, nitems_return, 
		&bytes_after, &prop) == Success)
	{
		if(actual_type == req_type)
			return (void *)prop;
		else
			XFree(prop);
	}
	return NULL;
}
#endif//EWMH

#ifdef EWMH
static unsigned long
handle_mwm_hints(Client * c)
{
	unsigned long nitems;
	const Atom mwm_hints = XA("_XA_MWM_HINTS");
	PropMwmHints *m;
	if((m=jbwm_get_property(c->window, mwm_hints, mwm_hints, &nitems)))
	{
		if(nitems >= PROP_MWM_HINTS_ELEMENTS 
			&& (m->flags & MWM_HINTS_DECORATIONS)
			&& !(m->decorations & MWM_DECOR_ALL)
			&& !(m->decorations & MWM_DECOR_BORDER))
			c->border = 0;
		XFree(m);
	}
	return nitems;
}
#endif//EWMH

static void
set_size(Client * c, const unsigned int width, const unsigned int height)
{
	c->geometry.width = width;
	c->geometry.height = height;
	send_configure(c);
}

static void
init_geometry_size(Client * c, XWindowAttributes * attr)
{
	const int awidth = attr->width;
	const int aheight = attr->height;

	if((awidth >= c->size.min_width)
		&& (aheight >= c->size.min_height))
		set_size(c, awidth, aheight);
	else
		set_size(c, c->size.base_width, c->size.base_height);
}

static void
set_position(Client * c, const int x, const int y)
{
	c->geometry.x = x;
	c->geometry.y = y;
	send_configure(c);
}

static void
init_geometry_position(Client * c, XWindowAttributes * attr)
{
	if(!c->size.flags) return;
	if((attr->map_state == IsViewable)
		|| c->size.flags & USPosition)
		set_position(c, attr->x, attr->y);
	else
	{
		const ScreenInfo *s = c->screen;

		if(!s)
			return;
		{
			Position p;
			get_mouse_position(s->root, (int *)&p.x, (int *)&p.y);
			const short mx = s->width;
			const short my = s->height;
			const ubyte b = c->border;
			const XRectangle *g = &(c->geometry);
			set_position(c, (p.x * (mx - b - g->width)) / mx,
				(p.y * (my - b - g->height)) / my);
		}
	}
}

#ifdef EWMH
static void
init_long_properties(Client * c, unsigned long *nitems)
{
	unsigned long *lprop=jbwm_get_property(c->window, 
		XA("_NET_WM_DESKTOP"), XA_CARDINAL, nitems);
	if(lprop && nitems && (lprop[0]<9))
		c->vdesk=lprop[0];
	XFree(lprop);
}
#endif//EWMH

#ifdef EWMH
static void
init_atom_properties(Client * c, unsigned long *nitems)
{
	Atom *aprop;

	if((aprop = jbwm_get_property(c->window, XA("WM_STATE"), 
		XA_ATOM, nitems)))
	{
		for(ubyte i = 0; i < *nitems; i++)
			if(aprop[i] == XA("_NET_WM_STATE_STICKY"))
				add_sticky(c);
		XFree(aprop);
	}
}
#endif//EWMH

static void
init_geometry_properties(Client * c)
{
	if(!c->screen)
	{
		return;
	}
	c->vdesk = c->screen->vdesk;
#ifdef EWMH
	unsigned long nitems = handle_mwm_hints(c);
	init_long_properties(c, &nitems);
	init_atom_properties(c, &nitems);
#endif//EWMH
	//remove_sticky(c);
}

static void
init_geometry(Client * c)
{
	initialize_client_ce(c);
	init_geometry_properties(c);
	XWindowAttributes attr;
	XGetWindowAttributes(D, c->window, &attr);
#ifdef USE_CMAP
	c->cmap = attr.colormap;
#endif /* USE_CMAP */
	XGetWMNormalHints(D, c->window, &(c->size), 
		&(attr.all_event_masks) /* dummy */);
	init_geometry_size(c, &attr);
	init_geometry_position(c, &attr);
	/* Test if the reparent that is to come 
	   would trigger an unmap event. */
	if(attr.map_state == IsViewable)
		c->ignore_unmap++;
}

static void
reparent(Client *c)
{
	const unsigned long vm= CWOverrideRedirect | CWEventMask;
	const ubyte s = c->screen->screen;
	XSetWindowAttributes a;

	a.override_redirect=true;
	a.event_mask = SubstructureRedirectMask | SubstructureNotifyMask 
		| ButtonPressMask | EnterWindowMask;
	c->parent=XCreateWindow(D, c->screen->root, 
		c->geometry.x, c->geometry.y, c->geometry.width, 
		c->geometry.height, c->border, DefaultDepth(D, s),
		CopyFromParent, DefaultVisual(D, s), vm, &a);
	XAddToSaveSet(D, c->window);
	XReparentWindow(D, c->window, c->parent, 0, 0);
	XMapWindow(D, c->window);
#ifdef EWMH
	set_ewmh_allowed_actions(c->window);
#endif//EWMH	
}

