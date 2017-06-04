// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "new.h"
#include <X11/Xatom.h> // for XA_CARDINAL
#include <stdlib.h>
#include "JBWMClient.h"
#include "client.h"
#include "config.h" // for JBWM_MAX_DESKTOPS
#include "ewmh.h"
#include "geometry.h"
#include "key_masks.h"
#include "log.h" // for JBWM_LOG
#include "mwm.h" // for jbwm_handle_mwm_hints
#include "screen.h"
#include "select.h"
#include "shape.h" // for jbwm_new_shaped_client
#include "util.h"
#ifdef JBWM_USE_EWMH
static uint8_t wm_desktop(Display * d, const Window w, uint8_t vdesk)
{
	uint16_t n;
	const Atom a = jbwm_ewmh_get_atom(JBWM_EWMH_WM_DESKTOP);
	unsigned long *lprop = jbwm_get_property(d, w, a, &n);
	if (lprop) {
		if (n && lprop[0] < JBWM_MAX_DESKTOPS) // is valid
			vdesk = lprop[0]; // Set vdesk to property value
		else // Set to a valid desktop number:
			jbwm_set_property(d, w, a, XA_CARDINAL, &vdesk, 1);
		XFree(lprop);
	}
	JBWM_LOG("wm_desktop(w: %d): vdesk is %d\n", (int) w, vdesk);
	return vdesk;
}
#endif//JBWM_USE_EWMH
__attribute__((nonnull))
static void init_properties(Display * d, struct JBWMClient * restrict c)
{
	jbwm_handle_mwm_hints(d, c);
	c->vdesk = jbwm_get_screen(c)->vdesk;
#ifdef JBWM_USE_EWMH
	c->vdesk = wm_desktop(d, c->window, c->vdesk);
#endif//JBWM_USE_EWMH
}
__attribute__((nonnull))
static Window get_parent(Display * d, struct JBWMClient * restrict c)
{
	enum {
		CFP = CopyFromParent,
		CW_VM = CWOverrideRedirect | CWEventMask,
		WA_EM = SubstructureRedirectMask | SubstructureNotifyMask |
			ButtonPressMask | EnterWindowMask
	};
	struct JBWMRectangle * g = &c->size;
	return XCreateWindow(d, jbwm_get_root(d, c), g->x, g->y,
		g->width, g->height, c->border, CFP, CFP,
		NULL, CW_VM, &(XSetWindowAttributes){
		.override_redirect=true, .event_mask = WA_EM});
}
static void reparent_window(Display * d, Window parent, Window window)
{
	XAddToSaveSet(d, window);
	XReparentWindow(d, window, parent, 0, 0);
	XMapWindow(d, window);
}
__attribute__((nonnull))
static void reparent(Display * d, struct JBWMClient * restrict c)
{
	JBWM_LOG("reparent()");
	jbwm_new_shaped_client(d, c);
	reparent_window(d, c->parent = get_parent(d, c), c->window);
	// Required by wm-spec:
	jbwm_set_frame_extents(d, c);
}
// Allocate the client structure with some defaults set
static struct JBWMClient * get_JBWMClient(const Window w,
	struct JBWMScreen * s)
{
	struct JBWMClient * restrict c = malloc(sizeof(struct JBWMClient));
	*c = (struct JBWMClient) {.screen = s->id, .window = w, .border = 1};
	return c;
}
// Grab input and setup JBWM_USE_EWMH for client window
static void do_grabs(Display * d, const Window w)
{
	// jbwm_ewmh_set_allowed_actions must come before jbwm_grab_buttons.
	jbwm_ewmh_set_allowed_actions(d, w);
	XSelectInput(d, w, EnterWindowMask | PropertyChangeMask
		| ColormapChangeMask);
	// keys to grab:
	jbwm_grab_button(d, w, jbwm_get_grab_mask());
}
void jbwm_new_client(Display * d, struct JBWMScreen * s, const Window w)
{
	JBWM_LOG("jbwm_new_client(..., w: %d)", (int)w);
	struct JBWMClient * restrict c = get_JBWMClient(w, s);
	jbwm_prepend_client(c);
	do_grabs(d, w);
	jbwm_set_client_geometry(d, c);
	init_properties(d, c);
	reparent(d, c);
	jbwm_restore_client(d, c);
	jbwm_select_client(d, c);
}
