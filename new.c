// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_SHAPE
#include <X11/extensions/shape.h>
#endif//USE_SHAPE
#include <X11/Xatom.h>
#include "atoms.h"
#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "jbwm.h"
#include "log.h"
#include "mwm.h"
#include "screen.h"
#include "titlebar.h"

#if defined(EWMH) || defined(MWM)
void *get_property(Window w, Atom property, Atom type,
	unsigned long * restrict num_items)
{
	assert(num_items);
	int actual_format;
	unsigned long bytes_after;
	unsigned char *prop;

	if (XGetWindowProperty(D, w, property, 0L, 1024, false, property,
			       &type, &actual_format, num_items, &bytes_after,
			       &prop) == Success)
		return prop;

	return NULL;
}
#endif//EWMH||MWM

__attribute__((nonnull))
static void init_size(Client * restrict c,
	XWindowAttributes * restrict attr)
{
	const Dim dim = { attr->width, attr->height };
	const bool valid = (dim.w >= c->size.min_width)
	    && (dim.h >= c->size.min_height);
	c->size.width = valid ? dim.w : c->size.min_width;
	c->size.height = valid ? dim.h : c->size.min_height;
}

__attribute__((nonnull))
static void init_position(Client * restrict c,
	XWindowAttributes * restrict attr)
{
	XSizeHints *restrict g = &(c->size);
	const bool a = (attr->map_state == IsViewable)
	    || (g->flags & USPosition);
	g->x = a ? attr->x : 0;
	g->y = a ? attr->y : TDIM;
}

#ifdef EWMH
__attribute__((nonnull))
static void wm_desktop(Client * restrict c)
{
	unsigned long nitems;
	unsigned long *lprop = get_property(c->window, ewmh.WM_DESKTOP,
					    XA_CARDINAL, &nitems);

	if (lprop && nitems && (lprop[0] < 9))
		c->vdesk = lprop[0];

	if (!lprop)
		XPROP(c->window, ewmh.WM_DESKTOP, XA_CARDINAL, &(c->vdesk), 1);
	else
		XFree(lprop);
}
#else//!EWMH
#define wm_desktop(c)
#endif//EWMH

/* These are corner cases at the moment, so this code is disabled.  If further
 * testing shows clients actually set any of these properties, it will be
 * further implemented. */
#if 0
#ifdef EWMH
static void init_wm_state(Client * restrict c)
{
	Atom *aprop;
	unsigned long nitems;
	if(!(aprop=get_property(c->window, XA("WM_STATE"), XA_ATOM, &nitems)))
		return;
	for (uint8_t i = 0; i < nitems; i++) {
		if (aprop[i] == ewmh.WM_STATE_STICKY) c->flags |= JB_STICKY;
		else if (aprop[i] == ewmh.WM_STATE_SHADED) shade(c);
		else if (aprop[i] == ewmh.WM_STATE_FULLSCREEN) set_fullscreen(c);
	}
	XFree(aprop);
}
#else//!EWMH
#define init_wm_state(c)
#endif//EWMH
#endif//0

__attribute__((nonnull))
static void init_properties(Client * restrict c)
{
	assert(c->screen);
	/*
	if (!c->screen)
		return;
	*/
	c->vdesk = c->screen->vdesk;
	handle_mwm_hints(c);
	wm_desktop(c);
	//init_wm_state(c);
}

#ifdef FIX_FIREFOX
static void fix_firefox(Client * restrict c)
{
	// Hack to make flash videos in firefox fullscreen: 
	char * name = get_title(c->window);
	if(!name) return;
	if(!strncmp(name, "plugin-container", 16)) {
		const uint8_t b = c->border; // save before next call
		set_fullscreen(c);
		c->size.x-=b;
		c->size.y-=b;
		moveresize(c);
	}
	XFree(name);
}
#else//!FIX_FIREFOX
#define fix_firefox(c)
#endif//FIX_FIREFOX

__attribute__((nonnull))
static void init_geometry(Client * restrict c)
{
	XWindowAttributes attr;
	XGetWindowAttributes(D, c->window, &attr);
	c->cmap = attr.colormap;
	{
		long d;// dummy var
		XGetWMNormalHints(D, c->window, &(c->size), &d);
	}
	init_size(c, &attr);
	init_position(c, &attr);

	// Test if the reparent that is to come would trigger an unmap event.
	if (attr.map_state == IsViewable)
		c->ignore_unmap++;
	fix_firefox(c); // fix flash plugin-container bug
}

#ifdef USE_SHAPE
__attribute__((pure))
static bool is_shaped(Client * c)
{
	int d, s;
#define U (unsigned int *)
	return XShapeQueryExtents(D, c->window, &s, &d, &d, U & d, U & d, &d,
				  &d, &d, U & d, U & d) && s;
}

static void setup_shaped(Client * restrict c)
{
	if (is_shaped(c)) {
		LOG("Window %d is shaped", (int)c->window);
		c->border = 0;
		c->flags |= JB_NO_TB | JB_SHAPED;
	}
}
#else//!USE_SHAPE
#define setup_shaped(c)
#endif//USE_SHAPE

static void reparent(Client * c)
{
	LOG("reparent()");
	setup_shaped(c);
	const unsigned long vm = CWOverrideRedirect | CWEventMask;
	const uint8_t s = c->screen->screen;
	XSetWindowAttributes a = {.override_redirect = true,
		.event_mask =
		    SubstructureRedirectMask | SubstructureNotifyMask |
		    ButtonPressMask | EnterWindowMask
	};
	XSizeHints *g = &(c->size);
	c->parent =
	    XCreateWindow(D, c->screen->root, g->x, g->y, g->width,
			  g->height, c->border, DefaultDepth(D, s),
			  CopyFromParent, DefaultVisual(D, s), vm, &a);
	const Window w = c->window;
	XAddToSaveSet(D, w);
	XReparentWindow(D, w, c->parent, 0, 0);
	XMapWindow(D, w);
}

void make_new_client(Window w, ScreenInfo * s)
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

	if (c->flags & JB_DONT_MANAGE)
		return;

	XSelectInput(D, c->window, EnterWindowMask | PropertyChangeMask 
		| ColormapChangeMask);
	set_shape(c);
	reparent(c);
	unhide(c);
	jbwm_grab_button(w, jbwm.keymasks.grab, AnyButton);
	set_ewmh_allowed_actions(w);
}
