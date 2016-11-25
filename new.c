// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "new.h"
#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "font.h"
#include "jbwm.h"
#include "keys.h"
#include "log.h"
#include "macros.h"
#include "mwm.h"
#include "screen.h"
#include "shape.h"
#include "title_bar.h"
#include "util.h"
#include <stdlib.h>
#include <X11/Xatom.h>
#define DEBUG_NEW
#ifndef DEBUG_NEW
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!DEBUG_NEW
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
	Display * d = c->d;
	jbwm_handle_mwm_hints(c);
	c->vdesk = jbwm_get_screens()[c->screen].vdesk;
	c->vdesk = wm_desktop(d, c->window, c->vdesk);
}
__attribute__((const))
static uint16_t get_per_min(uint16_t spec, uint16_t min)
{
	return (spec >= min) ? spec : min;
}
__attribute__((nonnull))
static void init_geometry(struct JBWMClient * c)
{
	XWindowAttributes a;
	Display * d = c->d;
	XGetWindowAttributes(d, c->window, &a);
	JBWM_LOG("XGetWindowAttributes() win: 0x%x, x: %d, y: %d, w: %d, h: %d",
		(int)c->window, a.x, a.y, a.width, a.height);
	c->cmap = a.colormap;
	bool pos = a.map_state == IsViewable;
	struct JBWMRectangle * g = &c->size;
	{ // h scope
		XSizeHints h;
		XGetWMNormalHints(d, c->window, &h, &(long){0});
		if (h.flags & USSize) {
			// if size hints provided, use them
			g->width = get_per_min(h.width, h.min_width);
			g->height = get_per_min(h.height, h.min_height);
		} else { // use existing window attributes
			g->width = a.width;
			g->height = a.height;
		}
		if (h.flags & USPosition)
			pos = true;
	}
	struct JBWMScreen * s = &jbwm_get_screens()[c->screen];
	// sanitize dimensions:
	g->width = JB_MIN(g->width, s->size.w);
	g->height = JB_MIN(g->height, s->size.h);
	g->x = pos ? a.x : (s->size.w >> 1) - (g->width >> 1);
	g->y = pos ? a.y : (s->size.h >> 1) - (g->height >> 1);
	// Test if the reparent that is to come would trigger an unmap event.
	c->ignore_unmap += a.map_state == IsViewable ? 1 : 0;
	JBWM_LOG("init_geometry() win: 0x%x, x: %d, y: %d, w: %d, h: %d",
		(int)c->window, g->x, g->y, g->width, g->height);
}
__attribute__((nonnull))
static Window get_parent(struct JBWMClient * c)
{
	return XCreateWindow(c->d, jbwm_get_screens()[c->screen].root,
		c->size.x, c->size.y, c->size.width, c->size.height,
		c->border, CopyFromParent, CopyFromParent, CopyFromParent,
		CWOverrideRedirect | CWEventMask, &(XSetWindowAttributes){
		.override_redirect=true, .event_mask
		= SubstructureRedirectMask | SubstructureNotifyMask
		| ButtonPressMask | EnterWindowMask});
}
__attribute__((nonnull))
static void reparent(struct JBWMClient * c)
{
	JBWM_LOG("reparent()");
	Display * d = c->d;
	jbwm_new_shaped_client(d, c);
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
static void do_grabs(Display * restrict d, const jbwm_window_t w)
{
	XSelectInput(d, w, EnterWindowMask
		| PropertyChangeMask | ColormapChangeMask);
	jbwm_grab_window_keys(d, w);
	jbwm_ewmh_set_allowed_actions(d, w);
}
void jbwm_new_client(Display * restrict d, struct JBWMScreen * restrict s,
	const jbwm_window_t w)
{
	JBWM_LOG("jbwm_new_client(..., w: %d)", (int)w);
	struct JBWMClient * c = get_JBWMClient(w, s);
	c->d = d;
	jbwm_set_head_client(c);
	do_grabs(d, w);
	init_geometry(c);
	init_properties(c);
	reparent(c);
	jbwm_restore_client(c);
}
