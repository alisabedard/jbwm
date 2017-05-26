// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "new.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include "JBWMSize.h"
#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "key_masks.h"
#include "log.h"
#include "macros.h"
#include "mwm.h"
#include "screen.h"
#include "select.h"
#include "shape.h"
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
static long handle_wm_normal_hints(Display * d, const Window win,
	const struct JBWMRectangle * restrict attribute_geometry,
	struct JBWMRectangle * restrict geometry_return)
{
	/* Though these statements may be combined, writing the following
	 * assignment this way ensures the conditional is only evaluated once.
	 * */
	XSizeHints h;
	if (XGetWMNormalHints(d, win, &h, &(long){0})
		&& (h.flags & USSize)) {
		// if size hints provided, use them
		geometry_return->width = JB_MAX(h.width, h.min_width);
		geometry_return->height = JB_MAX(h.height, h.min_height);
	} else { // use existing window attributes
		geometry_return->width = attribute_geometry->width;
		geometry_return->height = attribute_geometry->height;
	}
	return h.flags;
}
static void check_dimensions(struct JBWMRectangle * restrict g,
	const struct JBWMSize screen)
{
	g->width = JB_MIN(g->width, screen.width);
	g->height = JB_MIN(g->height, screen.height);
}
__attribute__((const))
static int16_t get_center(const uint16_t wh, const uint16_t swh)
{
	return (swh >> 1) - (wh >> 1);
}
static void center(struct JBWMRectangle * restrict g,
	const struct JBWMSize s)
{
	g->x = get_center(g->width, s.width);
	g->y = get_center(g->height, s.height);
}
// returns true if window is viewable
static bool get_window_attributes(Display * d, struct JBWMClient * restrict c,
	struct JBWMRectangle * restrict attribute_geometry)
{
	XWindowAttributes a;
	XGetWindowAttributes(d, c->window, &a);
	JBWM_LOG("XGetWindowAttributes() win: 0x%x,"
		"x: %d, y: %d, w: %d, h: %d",
		(int)c->window, a.x, a.y, a.width, a.height);
	c->cmap = a.colormap;
	*attribute_geometry = (struct JBWMRectangle){.x = a.x, .y = a.y,
		.width = a.width, .height = a.height};
	return a.map_state == IsViewable;
}
static void init_geometry_for_screen_size(Display * d, const Window window,
	struct JBWMRectangle * g, const struct JBWMRectangle * restrict
	attribute_geometry, const struct JBWMSize scr_sz)
{
	check_dimensions(g, scr_sz);
	const long flags = handle_wm_normal_hints(d, window,
		attribute_geometry, g);
	const bool user_specified_position = flags & USPosition;
	const bool nonzero_position = attribute_geometry->x ||
		attribute_geometry->y;
	if (user_specified_position && nonzero_position) {
		JBWM_LOG("\t\tPosition is set by hints.");
		g->x = attribute_geometry->x;
		g->y = attribute_geometry->y;
	} else // Position not specified
		center(g, scr_sz);
}
static void init_geometry_for_screen(Display * d,
	struct JBWMClient * c, struct JBWMRectangle * g,
	const struct JBWMRectangle * restrict attribute_geometry)
{
	struct JBWMScreen * s = jbwm_get_screen(c);
	if (!s) {
		g->x = g->y = 0;
		return;
	}
	init_geometry_for_screen_size(d, c->window, g,
		attribute_geometry, (struct JBWMSize){DisplayWidth(d, s->id),
			DisplayHeight(d, s->id)});
}
__attribute__((nonnull))
static void init_geometry(Display * d, struct JBWMClient * c)
{
	struct JBWMRectangle attribute_geometry;
	const bool viewable = get_window_attributes(d, c,
		&attribute_geometry);
	struct JBWMRectangle * g = &c->size;
	if (viewable) {
		/* Increment unmap event counter for the reparent event.  */
		++c->ignore_unmap;
		/* If the window is already on screen before the window
		   manager starts, simply save its geometry then return. */
		*g = attribute_geometry;
		return;
	}
	c->ignore_unmap += viewable ? 1 : 0;
	JBWM_LOG("\t\tVIEWABLE: %d", viewable);
	init_geometry_for_screen(d, c, g, &attribute_geometry);
	JBWM_LOG("init_geometry() win: 0x%x, x: %d, y: %d, w: %d, h: %d",
		(int)c->window, g->x, g->y, g->width, g->height);
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
	init_geometry(d, c);
	init_properties(d, c);
	reparent(d, c);
	jbwm_restore_client(d, c);
	jbwm_select_client(d, c);
}
