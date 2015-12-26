/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include <stdlib.h>
#include <string.h>
#include "jbwm.h"

#if defined(EWMH) || defined(MWM)
void *
get_property(Window w, Atom property, Atom type, unsigned long *num_items)
{
	int actual_format;
	unsigned long bytes_after;
	unsigned char *prop;

	if(XGetWindowProperty(D, w, property, 0L, 1024, false, 
		property, &type, &actual_format, num_items, 
		&bytes_after, &prop) == Success)
		return prop;
	return NULL;
}
#endif//EWMH||MWM

#ifdef MWM
static void
handle_mwm_hints(Client * c)
{
	const Atom mwm_hints = XA("_MOTIF_WM_HINTS");
	struct { unsigned long flags, functions, 
		decor, input_mode, status; } *m;
	unsigned long n;
	if(!(m=get_property(c->window, mwm_hints, mwm_hints, &n)))
		return;

// flags:
#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_HINTS_INPUT_MODE    (1L << 2)
#define MWM_HINTS_STATUS        (1L << 3)

// functions:
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

// decor:
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)

// input_mode:
#define MWM_INPUT_MODELESS                      0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL     1
#define MWM_INPUT_SYSTEM_MODAL                  2
#define MWM_INPUT_FULL_APPLICATION_MODAL        3

// status:
#define MWM_TEAROFF_WINDOW 1
	unsigned long f;
	// If this is set, only use specified decorations:
	if(m->flags & MWM_HINTS_DECORATIONS)
	{
		f=m->decor;
		if(f & MWM_DECOR_ALL)
			goto mwm_decor_all;
		if(!(f & MWM_DECOR_BORDER))
			c->flags|=JB_CLIENT_NO_BORDER;
		if(!(f & MWM_DECOR_RESIZEH))
			c->flags|=JB_CLIENT_NO_RESIZE;
		if(!(f & MWM_DECOR_TITLE))
			c->flags|=JB_CLIENT_NO_TB;
		if(!(f & MWM_DECOR_MENU))
			c->flags|=JB_CLIENT_NO_CLOSE;
		if(!(f & MWM_DECOR_MINIMIZE))
			c->flags|=JB_CLIENT_NO_MIN;
		if(!(f & MWM_DECOR_MAXIMIZE))
			c->flags|=JB_CLIENT_NO_MAX;
	}
	if(m->flags & MWM_HINTS_STATUS)
	{
		f=m->status;
		if(f & MWM_TEAROFF_WINDOW)
			c->flags|=JB_CLIENT_TEAROFF;
	}
	if(m->flags & MWM_HINTS_FUNCTIONS)
	{
		f=m->functions;
		if(f & MWM_FUNC_ALL)
			goto mwm_func_all;
                if(!(f & MWM_FUNC_RESIZE))
                        c->flags|=JB_CLIENT_NO_RESIZE;
                if(!(f & MWM_FUNC_RESIZE))
                        c->flags|=JB_CLIENT_NO_MOVE;
                if(!(f & MWM_FUNC_CLOSE))
                        c->flags|=JB_CLIENT_NO_CLOSE;
                if(!(f & MWM_FUNC_MINIMIZE))
                        c->flags|=JB_CLIENT_NO_MIN;
                if(!(f & MWM_FUNC_MAXIMIZE))
                        c->flags|=JB_CLIENT_NO_MAX;
	}
	if(m->flags & MWM_HINTS_INPUT_MODE)
	{
		if(m->input_mode)
			c->flags|=JB_CLIENT_MODAL;
	}
mwm_decor_all:
mwm_func_all:
	XFree(m);
	if(c->flags&JB_CLIENT_NO_BORDER)
		c->border=0;
	if(c->flags&JB_CLIENT_TEAROFF)
	{
		c->border=0;
		c->flags|=JB_CLIENT_NO_RESIZE|JB_CLIENT_NO_MIN
			|JB_CLIENT_NO_MAX;
	}
}
#endif//MWM

static void
set_size(Client * c, const unsigned int width, const unsigned int height)
{
	c->size.width = width;
	c->size.height = height;
	configure(c);
}

static void
init_size_size(Client * c, XWindowAttributes * attr)
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
	c->size.x = x;
	c->size.y = y;
	configure(c);
}

static void
init_size_position(Client * c, XWindowAttributes * attr)
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
			const XSizeHints *g = &(c->size);
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
		for(ubyte i = 0; i < nitems; i++)
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
init_size(Client * c)
{
	XWindowAttributes attr;
	XGetWindowAttributes(D, c->window, &attr);
#ifdef USE_CMAP
	c->cmap = attr.colormap;
#endif /* USE_CMAP */
	XGetWMNormalHints(D, c->window, &(c->size), 
		&(attr.all_event_masks) /* dummy */);
	init_size_size(c, &attr);
	init_size_position(c, &attr);
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
	XSizeHints *g=&(c->size);
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
	c->next = jbwm.head;
	jbwm.head = c;
	c->screen = s;
	c->window = w;
	c->border = JBWM_BORDER;
	init_properties(c);
	init_size(c);
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

