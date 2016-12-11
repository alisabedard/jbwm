// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "new.h"
#include <stdlib.h>
#include <X11/Xatom.h>
#include "client.h"
#include "config.h"
#include "display.h"
#include "ewmh.h"
#include "jbwm.h"
#include "keys.h"
#include "log.h"
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
static uint8_t wm_desktop(Display * d, const jbwm_window_t w, uint8_t vdesk)
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
#else//!JBWM_USE_EWMH
#define wm_desktop(d, w, vdesk) vdesk
#endif//JBWM_USE_EWMH
__attribute__((nonnull))
static void init_properties(struct JBWMClient * c)
{
	jbwm_handle_mwm_hints(c);
	c->vdesk = jbwm_get_screen(c)->vdesk;
	c->vdesk = wm_desktop(jbwm_get_display(), c->window, c->vdesk);
}
__attribute__((const))
static uint16_t get_per_min(uint16_t spec, uint16_t min)
{
	return (spec >= min) ? spec : min;
}
static bool process_size_hints(Display * d, const jbwm_window_t win,
	struct JBWMRectangle * g, const uint16_t a_w,
	const uint16_t a_h)
{
	XSizeHints h;
	XGetWMNormalHints(d, win, &h, &(long){0});
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
static void check_max_size(uint16_t * dim, const uint16_t scr_dim)
{
	*dim = JB_MIN(*dim, scr_dim);
}
static void check_dimensions(struct JBWMRectangle * g,
	const struct JBDim s)
{
	check_max_size(&g->width, s.w);
	check_max_size(&g->height, s.h);
}
static int16_t get_center(const uint16_t wh, const uint16_t swh)
{
	return (swh >> 1) - (wh >> 1);
}
static void center(struct JBWMRectangle * g, const struct JBDim s)
{
	g->x = get_center(g->width, s.w);
	g->y = get_center(g->height, s.h);
}
// returns true if window is viewable
static bool get_window_attributes(struct JBWMClient * c,
	struct JBWMRectangle * a_geo)
{
	XWindowAttributes a;
	XGetWindowAttributes(jbwm_get_display(), c->window, &a);
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
static void init_geometry(struct JBWMClient * c)
{
	struct JBWMRectangle a_geo;
	const bool viewable = get_window_attributes(c, &a_geo);
	struct JBWMRectangle * g = &c->size;
	if (viewable) {
		/* Test if the reparent that is to come
		   would trigger an unmap event. */
		++c->ignore_unmap;
		/* If the window is already on screen before the window
		   manager starts, simply save its geometry then return. */
		*g = a_geo;
		return;
	}
	c->ignore_unmap += viewable ? 1 : 0;
	//bool pos = viewable;
	JBWM_LOG("\t\tVIEWABLE: %d", viewable);
	const bool pos = process_size_hints(jbwm_get_display(), c->window, g,
		a_geo.width, a_geo.height);
	{ // scr_sz scope
		const struct JBDim scr_sz = jbwm_get_screen(c)->size;
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
static Window get_parent(struct JBWMClient * c)
{
	enum {
		CFP = CopyFromParent,
		CW_VM = CWOverrideRedirect | CWEventMask,
		WA_EM = SubstructureRedirectMask | SubstructureNotifyMask
		| ButtonPressMask | EnterWindowMask
	};
	struct JBWMRectangle * g = &c->size;
	return XCreateWindow(jbwm_get_display(), jbwm_get_root(c), g->x, g->y, g->width,
		g->height, c->border, CFP, CFP, CFP, CW_VM,
		&(XSetWindowAttributes){.override_redirect=true,
		.event_mask = WA_EM});
}
__attribute__((nonnull))
static void reparent(struct JBWMClient * c)
{
	JBWM_LOG("reparent()");
	Display * d = jbwm_get_display();
	jbwm_new_shaped_client(c);
	const jbwm_window_t p = c->parent = get_parent(c), w = c->window;
	XAddToSaveSet(d, w);
	XReparentWindow(d, w, p, 0, 0);
	XMapWindow(d, w);
}
// Allocate the client structure with some defaults set
static struct JBWMClient * get_JBWMClient(const jbwm_window_t w,
	struct JBWMScreen * s)
{
	struct JBWMClient * c = malloc(sizeof(struct JBWMClient));
	*c = (struct JBWMClient) {.screen = s->screen,
		.window = w, .border = 1,
		.next = jbwm_get_head_client()};
	return c;
}
// Grab input and setup JBWM_USE_EWMH for client window
static void do_grabs(Display * d, const jbwm_window_t w)
{
	XSelectInput(d, w, EnterWindowMask
		| PropertyChangeMask | ColormapChangeMask);
	jbwm_grab_window_keys(d, w);
	jbwm_ewmh_set_allowed_actions(d, w);
}
void jbwm_new_client(struct JBWMScreen * s, const jbwm_window_t w)
{
	JBWM_LOG("jbwm_new_client(..., w: %d)", (int)w);
	struct JBWMClient * c = get_JBWMClient(w, s);
	jbwm_set_head_client(c);
	do_grabs(jbwm_get_display(), w);
	init_geometry(c);
	init_properties(c);
	reparent(c);
	jbwm_restore_client(c);
	jbwm_select_client(c);
}
