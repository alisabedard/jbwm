/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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
	Client *c;

	c = calloc(1, sizeof(Client));
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
	XSelectInput(jbwm.X.dpy, c->window, EnterWindowMask 
		| PropertyChangeMask | ColormapChangeMask);
#ifdef USE_SHAPE
	set_shape(c);
#endif /* USE_SHAPE */
	reparent(c);
	unhide(c);
	jbwm_grab_button(w, jbwm.keymasks.grab, AnyButton);
}

static void *
jbwm_get_property(Window w, Atom property, Atom req_type,
	unsigned long *nitems_return)
{
	Atom actual_type;
	int actual_format;
	unsigned long bytes_after;
	unsigned char *prop;

	if(XGetWindowProperty(jbwm.X.dpy, w, property, 0L, 1024, false, 
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

static unsigned long
handle_mwm_hints(Client * c)
{
	PropMwmHints *m;
	unsigned long nitems;
	const Atom mwm_hints = GETATOM("_XA_MWM_HINTS");

	if((m=jbwm_get_property(c->window, mwm_hints, mwm_hints, &nitems)))
	{
		if(nitems >= PROP_MWM_HINTS_ELEMENTS 
			&& (m->flags & MWM_HINTS_DECORATIONS)
			&& !(m->decorations & MWM_DECOR_ALL)
			&& !(m->decorations & MWM_DECOR_BORDER))
		{
			c->border = 0;
		}
		XFree(m);
	}

	return nitems;
}

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
	if(!c->size.flags)
		return;
	if((attr->map_state == IsViewable)
		|| c->size.flags & USPosition)
		set_position(c, attr->x, attr->y);
	else
	{
		const ScreenInfo *s = c->screen;

		if(!s)
			return;
		{
			const short mx = s->width;
			const short my = s->height;
			const XRectangle *g = &(c->geometry);
			const ubyte b = c->border;

			Window root = s->root;
			Position p;

			get_mouse_position(root, (int *)&p.x, (int *)&p.y);
			set_position(c,
				(p.x * (mx - b - g->width)) / mx,
				(p.y * (my - b - g->height)) / my);
		}
	}
}

static void
init_long_properties(Client * c, unsigned long *nitems)
{
	unsigned long *lprop;

	if((lprop =
		jbwm_get_property(c->window, GETATOM("_NET_WM_DESKTOP"),
		XA_CARDINAL, nitems)))
	{
		if(*nitems && valid_vdesk(lprop[0]))
			c->vdesk = lprop[0];
		XFree(lprop);
	}
}

static void
init_atom_properties(Client * c, unsigned long *nitems)
{
	Atom *aprop;

	if((aprop =
		jbwm_get_property(c->window, GETATOM("WM_STATE"),
		XA_ATOM, nitems)))
	{
		unsigned long i;

		for(i = 0; i < *nitems; i++)
		{
			if(aprop[i] == GETATOM("_NET_WM_STATE_STICKY"))
			{
				add_sticky(c);
			}
		}
		XFree(aprop);
	}
}

static void
init_geometry_properties(Client * c)
{
	unsigned long nitems;
	nitems = handle_mwm_hints(c);
	if(!c->screen)
	{
		return;
	}
	c->vdesk = c->screen->vdesk;
	init_long_properties(c, &nitems);
	remove_sticky(c);
	init_atom_properties(c, &nitems);

}

static void
init_geometry(Client * c)
{
	XWindowAttributes attr;

	initialize_client_ce(c);
	init_geometry_properties(c);

	XGetWindowAttributes(jbwm.X.dpy, c->window, &attr);
#ifdef USE_CMAP
	c->cmap = attr.colormap;
#endif /* USE_CMAP */
	XGetWMNormalHints(jbwm.X.dpy, c->window, &(c->size), 
		&(attr.all_event_masks) /* dummy */);
	init_geometry_size(c, &attr);
	init_geometry_position(c, &attr);
	/* Test if the reparent that is to come 
	   would trigger an unmap event. */
	if(attr.map_state == IsViewable)
		c->ignore_unmap++;
}

static void
reparent(Client * c)
{
	const XRectangle *g = &(c->geometry);
	const ubyte b = c->border;
	const int x = g->x - b;
	const int y = g->y - b; 
	const Window w = c->window;
	const unsigned long valuemask = CWOverrideRedirect | CWBorderPixel 
#ifdef USE_SHAPE
		| CWBackPixel | CWEventMask;
#else
		| CWBackPixel | CWEventMask | CWBackPixel;
#endif /* USE_SHAPE */
	const int s=c->screen->screen;
	XSetWindowAttributes attr;
	Display *d;

	if(!c->screen)
		return;
	d=jbwm.X.dpy;
#ifndef USE_SHAPE
	attr.background_pixel = BlackPixel(d, s);
#endif /* !USE_SHAPE */
	attr.border_pixel = c->screen->bg.pixel;
	attr.override_redirect = true;
	attr.event_mask = ChildMask | ButtonPressMask | EnterWindowMask;
	c->parent = XCreateWindow(d, c->screen->root, x, y, g->width, 
		g->height, b, DefaultDepth(d, s), CopyFromParent, 
		DefaultVisual(d, s), valuemask, &attr);
	XAddToSaveSet(d, w);
	XSetWindowBorderWidth(d, w, 0);
#ifdef USE_TBAR
	if(!(c->flags & JB_CLIENT_NO_TB))
		update_titlebar(c);
#endif
	XReparentWindow(d, w, c->parent, 0, 0);
	XMapWindow(d, w);
}

