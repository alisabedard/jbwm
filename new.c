// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "new.h"

#include "config.h"
#include "client_t.h"
#include "ewmh.h"
#include "jbwmenv.h"
#include "log.h"
#include "mwm.h"
#include "screen.h"
#include "shape.h"
#include "util.h"

#include <assert.h>
#include <stdlib.h>
#include <X11/Xatom.h>

#ifdef EWMH
__attribute__((nonnull))
static void wm_desktop(const Window w, uint8_t * restrict vdesk)
{
	unsigned long nitems;
	unsigned long *lprop = get_property(w,
		ewmh[WM_DESKTOP], &nitems);

	if (lprop && nitems && (lprop[0] < 9))
		*vdesk = lprop[0];

	if (!lprop)
		XPROP(w, ewmh[WM_DESKTOP],
			XA_CARDINAL, vdesk, 1);
	else
		XFree(lprop);
}
#else//!EWMH
#define wm_desktop(c)
#endif//EWMH

__attribute__((nonnull))
static void init_properties(Client * c)
{
	handle_mwm_hints(c);
	c->vdesk = c->screen->vdesk;
	wm_desktop(c->window, &c->vdesk);
}

#ifdef FIX_FIREFOX
static void fix_firefox(Client * c)
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
static void init_geometry(Client * c)
{
	XWindowAttributes attr;
	XGetWindowAttributes(jbwm.dpy, c->window, &attr);
	c->cmap = attr.colormap;
	XGetWMNormalHints(jbwm.dpy, c->window, &(c->size), &(long){0});
	const Dim dim = { attr.width, attr.height };
	const bool valid = (dim.w >= c->size.min_width)
	    && (dim.h >= c->size.min_height);
	c->size.width = valid ? dim.w : c->size.min_width;
	c->size.height = valid ? dim.h : c->size.min_height;
	const bool pos = (attr.map_state == IsViewable)
	    || (c->size.flags & USPosition);
	c->size.x=pos ? attr.x : (c->screen->size.w>>1)-(c->size.width>>1);
	c->size.y=pos ? attr.y : (c->screen->size.h>>1)-(c->size.height>>1);

	// Test if the reparent that is to come would trigger an unmap event.
	if (attr.map_state == IsViewable)
		c->ignore_unmap++;
	fix_firefox(c); // fix flash plugin-container bug
}

static void reparent(Client * c) // use of restrict here is a bug
{
	LOG("reparent()");
	setup_shaped(c);
	c->parent = XCreateWindow(jbwm.dpy, c->screen->root,
		c->size.x, c->size.y,
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

void make_new_client(const Window w, ScreenInfo * s)
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
	XSelectInput(jbwm.dpy, c->window, EnterWindowMask
		| PropertyChangeMask | ColormapChangeMask);
	reparent(c);
	unhide(c);
	jbwm_grab_button(w, jbwm.keymasks.grab, AnyButton);
	set_ewmh_allowed_actions(w);
}
