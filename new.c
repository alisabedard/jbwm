// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "new.h"

#include "ewmh.h"
#include "JBWMEnv.h"
#include "log.h"
#include "mwm.h"
#include "screen.h"
#include "shape.h"
#include "util.h"

#include <stdlib.h>
#include <X11/Xatom.h>

#ifdef EWMH
static uint8_t wm_desktop(const jbwm_window_t w, uint8_t vdesk)
{
	uint16_t n;
	unsigned long *lprop = jbwm_get_property(w, ewmh[WM_DESKTOP], &n);
	if (lprop) {
		if (n && lprop[0] < JBWM_MAX_DESKTOPS) // is valid
			vdesk = lprop[0]; // Set vdesk to property value
		else // Set to a valid desktop number:
			jbwm_set_property(w, ewmh[WM_DESKTOP], XA_CARDINAL, &vdesk, 1);
		XFree(lprop);
	}
	LOG("wm_desktop(): vdesk is %d\n", vdesk);
	return vdesk;
}
#endif//EWMH

__attribute__((nonnull))
static void init_properties(struct JBWMClient * c)
{
	handle_mwm_hints(c);
	c->vdesk = c->screen->vdesk;
#ifdef EWMH
	c->vdesk = wm_desktop(c->window, c->vdesk);
	// Required by wm-spec 1.4:
	const uint8_t b = c->border;
	jbwm_set_property(c->window, ewmh[FRAME_EXTENTS], XA_CARDINAL,
		(&(jbwm_atom_t[]){b, b, b, b}), 4);
#endif//EWMH
}

__attribute__((nonnull))
static void init_geometry(struct JBWMClient * c)
{
	XWindowAttributes attr;
	XGetWindowAttributes(jbwm.d, c->window, &attr);
	c->cmap = attr.colormap;
	XGetWMNormalHints(jbwm.d, c->window, &(c->size), &(long){0});
	c->size.width = (attr.width >= c->size.min_width)
		? attr.width : c->size.min_width;
	c->size.height = (attr.height >= c->size.min_height)
		? attr.height : c->size.min_height;
	const bool pos = (attr.map_state == IsViewable)
	    || (c->size.flags & USPosition);
	c->size.x = pos ? attr.x : (c->screen->size.w >> 1)
		- (c->size.width >> 1);
	c->size.y = pos ? attr.y : (c->screen->size.h >> 1)
		- (c->size.height >> 1);

	// Test if the reparent that is to come would trigger an unmap event.
	c->ignore_unmap = attr.map_state == IsViewable;
}

__attribute__((nonnull))
static Window get_parent(struct JBWMClient * restrict c)
{
	return XCreateWindow(jbwm.d, c->screen->root, c->size.x, c->size.y,
		c->size.width, c->size.height, c->border, CopyFromParent,
		CopyFromParent, CopyFromParent, CWOverrideRedirect
		| CWEventMask, &(XSetWindowAttributes){
		.override_redirect=true, .event_mask
		= SubstructureRedirectMask | SubstructureNotifyMask
		| ButtonPressMask | EnterWindowMask});

}

__attribute__((nonnull))
static void reparent(struct JBWMClient * c) // use of restrict here is a bug
{
	LOG("reparent()");
	setup_shaped(c);
	const jbwm_window_t p = c->parent = get_parent(c), w = c->window;
	XAddToSaveSet(jbwm.d, w);
	XReparentWindow(jbwm.d, w, p, 0, 0);
	XMapWindow(jbwm.d, w);
}

// Allocate the client structure with some defaults set
static struct JBWMClient * get_JBWMClient(const jbwm_window_t w,
	struct JBWMScreen * s)
{
	struct JBWMClient * c = malloc(sizeof(struct JBWMClient));
	*c = (struct JBWMClient) {.screen = s, .window = w, .border = 1,
		.next = jbwm.head};
	return c;
}

// Grab input and setup EWMH for client window
static void do_grabs(const jbwm_window_t w)
{
	XSelectInput(jbwm.d, w, EnterWindowMask
		| PropertyChangeMask | ColormapChangeMask);
	jbwm_grab_button(w, jbwm.keymasks.grab, AnyButton);
	set_ewmh_allowed_actions(w);
}

void jbwm_new_client(const jbwm_window_t w, struct JBWMScreen * s)
{
	LOG("jbwm_new_client(%d,s)", (int)w);
	struct JBWMClient * c = jbwm.head = get_JBWMClient(w, s);
	do_grabs(w);
	init_properties(c);
	init_geometry(c);
	reparent(c);
	jbwm_restore_client(c);
}
