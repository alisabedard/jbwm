/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include <stdlib.h>
#include <string.h>
#include "jbwm.h"

#if defined(EWMH) || defined(MWM)
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
#endif//EWMH||MWM

#ifdef MWM
static void
handle_mwm_hints(Client * c)
{
	const Atom mwm_hints = XA("_XA_MWM_HINTS");
	PropMwmHints *m;
	unsigned long nitems;
	if((m=jbwm_get_property(c->window, mwm_hints, mwm_hints, &nitems)))
	{
		if(nitems >= PROP_MWM_HINTS_ELEMENTS 
			&& (m->flags & MWM_HINTS_DECORATIONS)
			&& !(m->decorations & MWM_DECOR_ALL)
			&& !(m->decorations & MWM_DECOR_BORDER))
			c->border = 0;
		XFree(m);
	}
}
#endif//MWM

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
	const int dim[]={attr->width, attr->height};
	if((dim[0] >= c->size.min_width)
		&& (dim[1] >= c->size.min_height))
		set_size(c, dim[0], dim[1]);
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

		if(s)
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
init_long_properties(Client * c)
{
	unsigned long nitems;
	unsigned long *lprop=jbwm_get_property(c->window, ewmh.WM_DESKTOP, 
		XA_CARDINAL, &nitems);
	if(lprop && nitems && (lprop[0]<9))
		c->vdesk=lprop[0];
	if(!lprop)
		XPROP(c->window, ewmh.WM_DESKTOP, XA_CARDINAL, &(c->vdesk), 1);
	else
		XFree(lprop);
}

static void
init_atom_properties(Client * c)
{
	Atom *aprop;
	unsigned long nitems;
	if((aprop = jbwm_get_property(c->window, XA("WM_STATE"), 
		XA_ATOM, &nitems)))
	{
		for(ubyte i = 0; i < nitems; i++)
		{
			if(aprop[i] == ewmh.WM_STATE_STICKY)
				add_sticky(c);
			else if(aprop[i] == ewmh.WM_STATE_SHADED)
				shade(c);
		}
		XFree(aprop);
	}
}
#endif//EWMH

static void
init_properties(Client * c)
{
	if(!c->screen) return;
	c->vdesk = c->screen->vdesk;
#ifdef MWM
	handle_mwm_hints(c);
#endif//MWM
#ifdef EWMH
	init_long_properties(c);
	init_atom_properties(c);
#endif//EWMH
}

static void
init_geometry(Client * c)
{
	initialize_client_ce(c);
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
	XSetWindowAttributes a={.override_redirect=true,
		.event_mask=SubstructureRedirectMask | SubstructureNotifyMask 
		| ButtonPressMask | EnterWindowMask};
	XRectangle *g=&(c->geometry);
	c->parent=XCreateWindow(D, c->screen->root, g->x, g->y, g->width,
		g->height, c->border, DefaultDepth(D, s), CopyFromParent, 
		DefaultVisual(D, s), vm, &a);
	const Window w=c->window;
	XAddToSaveSet(D, w);
	XReparentWindow(D, w, c->parent, 0, 0);
	XMapWindow(D, w);
}

static Client *
Client_new(Window w, ScreenInfo * s)
{
	Client *c = calloc(1, sizeof(Client));
	c->next = head_client;
	head_client = c;
	c->screen = s;
	c->window = w;
	c->window_type = None;
	c->border = JBWM_BORDER;
	init_properties(c);
	init_geometry(c);
	return c;
}

void
make_new_client(Window w, ScreenInfo * s)
{
	Client *c=Client_new(w, s);
	if(c->flags & JB_CLIENT_DONT_MANAGE)
		return;
	long mask=EnterWindowMask|PropertyChangeMask;
#ifdef USE_CMAP
	mask|=ColormapChangeMask;
#endif//USE_CMAP
	XSelectInput(D, c->window, mask);
#ifdef USE_SHAPE
	set_shape(c);
#endif /* USE_SHAPE */
	reparent(c);
	unhide(c);
	jbwm_grab_button(w, jbwm.keymasks.grab, AnyButton);
#ifdef EWMH
	set_ewmh_allowed_actions(w);
#endif//EWMH	
}

