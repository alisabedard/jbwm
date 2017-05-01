// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "new.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include "client.h"
#include "config.h"
#include "display.h"
#include "ewmh.h"
#include "keys.h"
#include "macros.h"
#include "mwm.h"
#include "screen.h"
#include "select.h"
#include "shape.h"
#include "util.h"
//#define JBWM_DEBUG_NEW
#ifndef JBWM_DEBUG_NEW
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!JBWM_DEBUG_NEW
#ifdef JBWM_USE_EWMH
static uint8_t wm_desktop(Display * d, const Window w, uint8_t vdesk)
{
	uint16_t n;
	unsigned long *lprop = jbwm_get_property(d, w,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_DESKTOP), &n);
	if (lprop) {
		if (n && lprop[0] < JBWM_MAX_DESKTOPS) // is valid
			vdesk = lprop[0]; // Set vdesk to property value
		else // Set to a valid desktop number:
			jbwm_set_property(d, w,
				jbwm_ewmh_get_atom(JBWM_EWMH_WM_DESKTOP),
				XA_CARDINAL, &vdesk, 1);
		XFree(lprop);
	}
	JBWM_LOG("wm_desktop(w: %d): vdesk is %d\n", (int) w, vdesk);
	return vdesk;
}
#endif//JBWM_USE_EWMH
__attribute__((nonnull))
static void init_properties(Display * d, struct JBWMClient * restrict c)
{
	jbwm_handle_mwm_hints(c);
	c->vdesk = jbwm_get_screen(c)->vdesk;
#ifdef JBWM_USE_EWMH
	c->vdesk = wm_desktop(d, c->window, c->vdesk);
#endif//JBWM_USE_EWMH
}
__attribute__((const))
static uint16_t get_per_min(uint16_t spec, uint16_t min)
{
	return (spec >= min) ? spec : min;
}
static bool do_hints(Display * d, const Window win,
	struct JBWMRectangle * g, const uint16_t a_w,
	const uint16_t a_h)
{
	XSizeHints h;
	XGetWMNormalHints(d, win, &h, &(long){0});
	/* Though these statements may be combined, writing the following
	 * assignment this way ensures the conditional is only evaluated once.
	 * */
	if (h.flags & USSize) {
		// if size hints provided, use them
		g->width = get_per_min(h.width, h.min_width);
		g->height = get_per_min(h.height, h.min_height);
	} else { // use existing window attributes
		g->width = a_w;
		g->height = a_h;
	}
	return (h.flags & USPosition);
}
static void check_dimensions(struct JBWMRectangle * restrict g,
	const struct JBWMSize s)
{
	g->width = JB_MIN(g->width, s.width);
	g->height = JB_MIN(g->height, s.height);
}
__attribute__((const))
static int16_t get_center(const uint16_t wh, const uint16_t swh)
{
	return (swh >> 1) - (wh >> 1);
}
static void center(struct JBWMRectangle * g, const struct JBWMSize s)
{
	g->x = get_center(g->width,s.width);
	g->y = get_center(g->height,s.height);
}
// returns true if window is viewable
static bool get_window_attributes(Display * d,
	struct JBWMClient * restrict c,
	struct JBWMRectangle * a_geo)
{
	XWindowAttributes a;
	XGetWindowAttributes(d, c->window, &a);
	JBWM_LOG("XGetWindowAttributes() win: 0x%x,"
		"x: %d, y: %d, w: %d, h: %d",
		(int)c->window, a.x, a.y, a.width, a.height);
	c->cmap = a.colormap;
	a_geo->x = a.x;
	a_geo->y = a.y;
	a_geo->width = a.width;
	a_geo->height = a.height;
	return a.map_state == IsViewable;
}
__attribute__((nonnull))
static void init_geometry(Display * d, struct JBWMClient * restrict c)
{
	struct JBWMRectangle a_geo;
	const bool viewable = get_window_attributes(d, c, &a_geo);
	struct JBWMRectangle * g = &c->size;
	if (viewable) {
		/* Increment unmap event counter for the reparent event.  */
		++c->ignore_unmap;
		/* If the window is already on screen before the window
		   manager starts, simply save its geometry then return. */
		*g = a_geo;
		return;
	}
	c->ignore_unmap += viewable ? 1 : 0;
	//bool pos = viewable;
	JBWM_LOG("\t\tVIEWABLE: %d", viewable);
	const bool pos = do_hints(d, c->window, g,
		a_geo.width, a_geo.height);
	{ // *s, scr_sz scope
		struct JBWMScreen * s = jbwm_get_screen(c);
		if (!s) {
			g->x = g->y = 0;
			return;
		}
		const struct JBWMSize scr_sz = s->size;
		check_dimensions(g, scr_sz);
		if (pos && (a_geo.x || a_geo.y)) {
			JBWM_LOG("\t\tPosition is set by hints.");
			g->x = a_geo.x;
			g->y = a_geo.y;
		} else
			center(g, scr_sz);
	}
	JBWM_LOG("init_geometry() win: 0x%x, x: %d, y: %d, w: %d, h: %d",
		(int)c->window, g->x, g->y, g->width, g->height);
}
__attribute__((nonnull))
static Window get_parent(Display * d, struct JBWMClient * restrict c)
{
	enum {
		CFP = CopyFromParent,
		CW_VM = CWOverrideRedirect | CWEventMask,
		WA_EM = SubstructureRedirectMask | SubstructureNotifyMask
		| ButtonPressMask | EnterWindowMask
	};
	struct JBWMRectangle * g = &c->size;
	return XCreateWindow(d, jbwm_get_root(c), g->x, g->y,
		g->width, g->height, c->border, CFP, CFP,
		NULL, CW_VM, &(XSetWindowAttributes){
		.override_redirect=true, .event_mask = WA_EM});
}
__attribute__((nonnull))
static void reparent(Display * d, struct JBWMClient * restrict c)
{
	JBWM_LOG("reparent()");
	jbwm_new_shaped_client(c);
	const Window p = c->parent = get_parent(d, c), w = c->window;
	XAddToSaveSet(d, w);
	XReparentWindow(d, w, p, 0, 0);
	XMapWindow(d, w);
	// Required by wm-spec:
	jbwm_set_frame_extents(c);
}
// Allocate the client structure with some defaults set
static struct JBWMClient * get_JBWMClient(const Window w,
	struct JBWMScreen * s)
{
	struct JBWMClient * restrict c = malloc(sizeof(struct JBWMClient));
	*c = (struct JBWMClient) {.screen = s->screen,
		.window = w, .border = 1};
	return c;
}
// Grab input and setup JBWM_USE_EWMH for client window
static void do_grabs(Display * d, const Window w)
{
	enum {
		MASK = EnterWindowMask | PropertyChangeMask
			| ColormapChangeMask
	};
	XSelectInput(d, w, MASK);
	jbwm_grab_window_keys(d, w);
	jbwm_ewmh_set_allowed_actions(d, w);
}
void jbwm_new_client(struct JBWMScreen * s, const Window w)
{
	JBWM_LOG("jbwm_new_client(..., w: %d)", (int)w);
	struct JBWMClient * restrict c = get_JBWMClient(w, s);
	jbwm_prepend_client(c);
	Display * d = jbwm_get_display();
	do_grabs(d, w);
	init_geometry(d, c);
	init_properties(d, c);
	reparent(d, c);
	jbwm_restore_client(c);
	jbwm_select_client(c);
}
