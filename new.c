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
#include "titlebar.h"

#include <stdlib.h>
#include <X11/Xatom.h>

#ifdef EWMH
static uint8_t wm_desktop(const Window w, uint8_t vdesk)
{
	size_t n;
	unsigned long *lprop = get_property(w, ewmh[WM_DESKTOP], &n);
	if(lprop && n && lprop[0] < DESKTOPS)
		vdesk = lprop[0];
	else
		  XPROP(w, ewmh[WM_DESKTOP], XA_CARDINAL, &vdesk, 1);
	if(lprop)
		  XFree(lprop);
	LOG("wm_desktop(): vdesk is %d\n", vdesk);
	return vdesk;
}
#endif//EWMH

__attribute__((nonnull))
static void init_properties(Client * c)
{
	handle_mwm_hints(c);
	c->vdesk = c->screen->vdesk;
#ifdef EWMH
	c->vdesk = wm_desktop(c->window, c->vdesk);
	// Required by wm-spec 1.4:
	XPROP(c->window, ewmh[FRAME_EXTENTS], XA_CARDINAL,
		(&(Atom[]){1, 1, 1+TDIM, 1}), 4);
#endif//EWMH
}

__attribute__((nonnull))
static void init_geometry(Client * c)
{
	XWindowAttributes attr;
	XGetWindowAttributes(jbwm.dpy, c->window, &attr);
	c->cmap = attr.colormap;
	XGetWMNormalHints(jbwm.dpy, c->window, &(c->size), &(long){0});
	c->size.width = (attr.width >= c->size.min_width)
		? attr.width : c->size.min_width;
	c->size.height = (attr.height >= c->size.min_height)
		? attr.height : c->size.min_height;
	const bool pos = (attr.map_state == IsViewable)
	    || (c->size.flags & USPosition);
	c->size.x=pos ? attr.x : (c->screen->size[0]>>1)-(c->size.width>>1);
	c->size.y=pos ? attr.y : (c->screen->size[1]>>1)-(c->size.height>>1);

	// Test if the reparent that is to come would trigger an unmap event.
	c->ignore_unmap=attr.map_state==IsViewable;
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
	Client *c = calloc(1, sizeof(Client));
	XGrabServer(jbwm.dpy);
	c->next = jbwm.head;
	jbwm.head = c;
	c->screen = s;
	c->window = w;
	c->border = 1;
	init_properties(c);
	init_geometry(c);
	XSelectInput(jbwm.dpy, c->window, EnterWindowMask
		| PropertyChangeMask | ColormapChangeMask);
	reparent(c);
	unhide(c);
	jbwm_grab_button(w, jbwm.keymasks.grab, AnyButton);
	set_ewmh_allowed_actions(w);
	XUngrabServer(jbwm.dpy);
}
