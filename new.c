// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

#if defined(EWMH) || defined(MWM)
void *
get_property(Window w, Atom property, Atom type, unsigned long *num_items)
{
	assert(num_items);
	int actual_format;
	unsigned long int8_ts_after;
	unsigned char *prop;

	if(XGetWindowProperty(D, w, property, 0L, 1024, false, 
		property, &type, &actual_format, num_items, 
		&int8_ts_after, &prop) == Success)
		return prop;
	return NULL;
}
#endif//EWMH||MWM

static void
init_size(Client * c, XWindowAttributes * attr)
{
	const int dim[]={attr->width, attr->height};
	if((dim[0] >= c->size.min_width)
		&& (dim[1] >= c->size.min_height))
	{
		c->size.width=dim[0];
		c->size.height=dim[1];
	}
	else
	{
		c->size.width=c->size.min_width;
		c->size.height=c->size.min_height;
	}
}

static void
init_position(Client * c, XWindowAttributes * attr)
{
	XSizeHints *g=&(c->size);
	const bool a=(attr->map_state==IsViewable) || (g->flags & USPosition);
	g->x=a?attr->x:0;
	g->y=a?attr->y:TDIM;
}

#ifdef EWMH
static void
init_long_properties(Client * c)
{
	unsigned long nitems;
	unsigned long *lprop=get_property(c->window, ewmh.WM_DESKTOP, 
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
	if((aprop = get_property(c->window, XA("WM_STATE"), 
		XA_ATOM, &nitems)))
	{
		for(uint8_t i = 0; i < nitems; i++)
		{
			if(aprop[i] == ewmh.WM_STATE_STICKY)
				add_sticky(c);
#ifdef USE_TBAR
			else if(aprop[i] == ewmh.WM_STATE_SHADED)
				shade(c);
#endif//USE_TBAR
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
	XWindowAttributes attr;
	XGetWindowAttributes(D, c->window, &attr);
#ifdef USE_CMAP
	c->cmap = attr.colormap;
#endif//USE_CMAP
	long d; // dummy var
	XGetWMNormalHints(D, c->window, &(c->size), &d);
	init_size(c, &attr);
	init_position(c, &attr);
	// Test if the reparent that is to come would trigger an unmap event.
	if(attr.map_state == IsViewable)
		c->ignore_unmap++;
}

#ifdef USE_SHAPE
static bool
is_shaped(Client *c)
{
	int d, s;
#define U (unsigned int *)
	return XShapeQueryExtents(D, c->window, &s, &d, &d, U&d, 
		U&d, &d, &d, &d, U&d, U&d) && s;
}
#endif//USE_SHAPE

static void
reparent(Client * c)
{
	LOG("reparent()");
#ifdef USE_SHAPE
	if(is_shaped(c))
	{
		LOG("Window %d is shaped", (int)c->window);
		c->border=0;
		c->flags |= JB_NO_TB | JB_SHAPED;
	}
#endif//USE_SHAPE
	const unsigned long vm= CWOverrideRedirect | CWEventMask;
	const uint8_t s = c->screen->screen;
	XSetWindowAttributes a={.override_redirect=true,
		.event_mask=SubstructureRedirectMask | SubstructureNotifyMask 
		| ButtonPressMask | EnterWindowMask};
	XSizeHints * g=&(c->size);
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
	LOG("Client_new(%d,s)", (int)w);
	Client *c = calloc(1, sizeof(Client));
	assert(c);
	c->next = jbwm.head;
	jbwm.head = c;
	c->screen = s;
	c->window = w;
	c->border = JBWM_BORDER;
	init_properties(c);
	init_geometry(c);
	return c;
}

void
make_new_client(Window w, ScreenInfo * s)
{
	Client *c=Client_new(w, s);
	if(c->flags & JB_DONT_MANAGE)
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

