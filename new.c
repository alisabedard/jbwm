// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#include "atoms.h"
#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "jbwm.h"
#include "log.h"
#include "mwm.h"
#include "screen.h"
#include "shape.h"
#include "titlebar.h"

#if defined(EWMH) || defined(MWM)
void *get_property(Window w, Atom property, Atom type,
	unsigned long * restrict num_items)
{
	assert(num_items);
	unsigned char *prop;

	if (XGetWindowProperty(jbwm.dpy, w, property, 0L, 1024, false, property,
			       &type, &(int){0}, num_items, 
			       &(long unsigned int){0}, 
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
	unsigned long *lprop = get_property(c->window, 
		ewmh.atoms[WM_DESKTOP], XA_CARDINAL, &nitems);

	if (lprop && nitems && (lprop[0] < 9))
		c->vdesk = lprop[0];

	if (!lprop)
		XPROP(c->window, ewmh.atoms[WM_DESKTOP],
			XA_CARDINAL, &(c->vdesk), 1);
	else
		XFree(lprop);
}
#else//!EWMH
#define wm_desktop(c)
#endif//EWMH

__attribute__((nonnull))
static void init_properties(Client * restrict c)
{
	assert(c->screen);
	c->vdesk = c->screen->vdesk;
	handle_mwm_hints(c);
	wm_desktop(c);
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
	XGetWindowAttributes(jbwm.dpy, c->window, &attr);
	c->cmap = attr.colormap;
	XGetWMNormalHints(jbwm.dpy, c->window, &(c->size), &(long){0});
	init_size(c, &attr);
	init_position(c, &attr);

	// Test if the reparent that is to come would trigger an unmap event.
	if (attr.map_state == IsViewable)
		c->ignore_unmap++;
	fix_firefox(c); // fix flash plugin-container bug
}

static void reparent(Client * restrict c)
{
	LOG("reparent()");
	setup_shaped(c);
	c->parent = XCreateWindow(jbwm.dpy, c->screen->root, c->size.x, c->size.y, 
		c->size.width, c->size.height, c->border,
		CopyFromParent, CopyFromParent, CopyFromParent, 
		CWOverrideRedirect | CWEventMask, &(XSetWindowAttributes){
			.override_redirect=true,
			.event_mask = SubstructureRedirectMask
				| SubstructureNotifyMask
				| ButtonPressMask
				| EnterWindowMask});
	XAddToSaveSet(jbwm.dpy, c->window);
	XReparentWindow(jbwm.dpy, c->window, c->parent, 0, 0);
	XMapWindow(jbwm.dpy, c->window);
}

void make_new_client(Window w, ScreenInfo * s)
{	
	LOG("make_new_client(%d,s)", (int)w);
	assert(s);
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

	XSelectInput(jbwm.dpy, c->window, EnterWindowMask | PropertyChangeMask 
		| ColormapChangeMask);
	reparent(c);
	unhide(c);
	jbwm_grab_button(w, jbwm.keymasks.grab, AnyButton);
	set_ewmh_allowed_actions(w);
}
