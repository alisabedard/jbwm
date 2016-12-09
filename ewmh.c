// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "ewmh.h"
#include <string.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include "client.h"
#include "config.h"
#include "font.h"
#include "log.h"
#include "macros.h"
#include "util.h"
//#define JBWM_DEBUG_EWMH
#ifndef JBWM_DEBUG_EWMH
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!JBWM_DEBUG_EWMH
static Atom jbwm_ewmh[JBWM_EWMH_ATOMS_COUNT];
jbwm_atom_t jbwm_ewmh_get_atom(const uint8_t index)
{
	return jbwm_ewmh[index];
}
static void jbwm_ewmh_init(Display * restrict d)
{
	static char * atom_names [] = { // This list must match 1:1 with enum
		"_NET_SUPPORTED",
		"_NET_CURRENT_DESKTOP",
		"_NET_NUMBER_OF_DESKTOPS",
		"_NET_DESKTOP_VIEWPORT",
		"_NET_DESKTOP_GEOMETRY",
		"_NET_SUPPORTING_WM_CHECK",
		"_NET_ACTIVE_WINDOW",
		"_NET_MOVERESIZE_WINDOW",
		"_NET_CLOSE_WINDOW",
		"_NET_CLIENT_LIST",
		"_NET_VIRTUAL_ROOTS",
		"_NET_CLIENT_LIST_STACKING",
		"_NET_FRAME_EXTENTS",
		"_NET_WM_ALLOWED_ACTIONS",
		"_NET_WM_NAME",
		"_NET_WM_DESKTOP",
		"_NET_WM_MOVERESIZE",
		"_NET_WM_PID",
		"_NET_WM_WINDOW_TYPE",
		"_NET_WM_ACTION_MOVE",
		"_NET_WM_ACTION_RESIZE",
		"_NET_WM_ACTION_CLOSE",
		"_NET_WM_ACTION_SHADE",
		"_NET_WM_ACTION_FULLSCREEN",
		"_NET_WM_ACTION_CHANGE_DESKTOP",
		"_NET_WM_ACTION_ABOVE",
		"_NET_WM_ACTION_BELOW",
		"_NET_WM_ACTION_MAXIMIZE_HORZ",
		"_NET_WM_ACTION_MAXIMIZE_VERT",
		"_NET_WM_STATE",
		"_NET_WM_STATE_STICKY",
		"_NET_WM_STATE_MAXIMIZED_VERT",
		"_NET_WM_STATE_MAXIMIZED_HORZ",
		"_NET_WM_STATE_SHADED",
		"_NET_WM_STATE_HIDDEN",
		"_NET_WM_STATE_FULLSCREEN",
		"_NET_WM_STATE_ABOVE",
		"_NET_WM_STATE_BELOW",
		"_NET_WM_STATE_FOCUSED",
	};
	XInternAtoms(d, atom_names, JBWM_EWMH_ATOMS_COUNT, false, jbwm_ewmh);
}
static inline void wprop(Display * d, const jbwm_window_t win,
	const enum JBWMAtomIndex i, const jbwm_atom_t type,
	void * data, const uint16_t size)
{
	jbwm_set_property(d, win, jbwm_ewmh[i], type, data, size);
}
static Window * get_mixed_client_list(Display * restrict d)
{
	enum {MAX_CLIENTS = 64};
	static Window l[MAX_CLIENTS];
	uint8_t n = 0;
	for (struct JBWMClient * i = jbwm_get_head_client();
		i && n < MAX_CLIENTS; i = i->next)
		l[n++] = i->window;
	wprop(d, DefaultRootWindow(d), JBWM_EWMH_CLIENT_LIST,
		XA_WINDOW, l, n);
	return l;
}
static Window * get_ordered_client_list(Display * restrict d)
{
	enum {MAX_CLIENTS= 64};
	static Window window_list[MAX_CLIENTS];
	// get ordered list of all windows on default screen:
	unsigned int n = 0;
	{ // * wl, nil scope
		Window * wl, nil;
		if (XQueryTree(d, DefaultRootWindow(d), &nil, &nil, &wl, &n)) {
			n = JB_MIN(n, MAX_CLIENTS); // limit to MAX_CLIENTS
			memcpy(window_list, wl, n * sizeof(Window));
			XFree(wl);
		}
	}
	JBWM_LOG("get_ordered_client_list() n: %d", n);
	wprop(d, DefaultRootWindow(d), JBWM_EWMH_CLIENT_LIST_STACKING,
		XA_WINDOW, window_list, n);
	return window_list;
}
void jbwm_ewmh_update_client_list(Display * restrict d)
{
	get_mixed_client_list(d);
	get_ordered_client_list(d);
}
void jbwm_ewmh_set_allowed_actions(Display * restrict d,
	const jbwm_window_t w)
{
	Atom a[] = {
		jbwm_ewmh[JBWM_EWMH_WM_ALLOWED_ACTIONS],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_MOVE],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_RESIZE],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_CLOSE],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_SHADE],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_FULLSCREEN],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_CHANGE_DESKTOP],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_ABOVE],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_BELOW],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_MAXIMIZE_HORZ],
		jbwm_ewmh[JBWM_EWMH_WM_ACTION_MAXIMIZE_VERT]
	};
	jbwm_set_property(d, w, a[0], XA_ATOM, &a, sizeof(a) / sizeof(Atom));
}
static void init_desktops(struct JBWMScreen * restrict s)
{
	Display * restrict d = s->d;
	const jbwm_window_t r = s->r;
	wprop(d, r, JBWM_EWMH_DESKTOP_GEOMETRY, XA_CARDINAL,
		&s->size, 2);
	wprop(d, r, JBWM_EWMH_CURRENT_DESKTOP, XA_CARDINAL,
		&s->vdesk, 1);
	wprop(d, r, JBWM_EWMH_NUMBER_OF_DESKTOPS, XA_CARDINAL,
		&(long){JBWM_MAX_DESKTOPS}, 1);
	wprop(d, r, JBWM_EWMH_DESKTOP_VIEWPORT, XA_CARDINAL,
		(&(long[]){0, 0}), 2);
	/* Pass the address of the field within s to keep scope.  */
	wprop(d, r, JBWM_EWMH_VIRTUAL_ROOTS, XA_WINDOW, &s->r, 1);
}
static jbwm_window_t init_supporting(Display * restrict d,
	const jbwm_window_t r)
{
	jbwm_window_t w = XCreateSimpleWindow(d, r, 0, 0, 1, 1, 0, 0, 0);
	wprop(d, r, JBWM_EWMH_SUPPORTING_WM_CHECK, XA_WINDOW, &w, 1);
	wprop(d, w, JBWM_EWMH_SUPPORTING_WM_CHECK, XA_WINDOW, &w, 1);
	wprop(d, w, JBWM_EWMH_WM_NAME, XA_STRING, JBWM_NAME,
		sizeof(JBWM_NAME));
	wprop(d, w, JBWM_EWMH_WM_PID, XA_CARDINAL,
		&(pid_t){getpid()}, 1);
	return w;
}
void jbwm_ewmh_init_screen(struct JBWMScreen * restrict s)
{
	Display * d = s->d;
	if (!jbwm_ewmh[0])
		jbwm_ewmh_init(d);
	jbwm_window_t r = s->root;
	jbwm_set_property(d, r, jbwm_ewmh[JBWM_EWMH_SUPPORTED],
		XA_ATOM, jbwm_ewmh, JBWM_EWMH_ATOMS_COUNT);
	jbwm_set_property(d, r, jbwm_ewmh[JBWM_EWMH_WM_NAME],
		XA_STRING, "jbwm", 4);
	// Set this to the root window until we have some clients.
	jbwm_set_property(d, r, jbwm_ewmh[JBWM_EWMH_CLIENT_LIST],
		XA_WINDOW, &r, 1);
	init_desktops(s);
	s->supporting = init_supporting(d, r);
}
void jbwm_set_frame_extents(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_set_frame_extents()");
	// left, right, top, bottom
	const uint8_t b = c->border;
#ifdef JBWM_USE_TITLE_BAR
	const uint8_t t = c->tb.win ? jbwm_get_font_height() : b;
	Atom f[] = {b, b, t, b};
#else//!JBWM_USE_TITLE_BAR
	Atom f[] = {b, b, b, b};
#endif//JBWM_USE_TITLE_BAR
	Display * d = c->d;
	jbwm_set_property(d, c->window, jbwm_ewmh[JBWM_EWMH_FRAME_EXTENTS],
		XA_CARDINAL, &f, 4);
	XFlush(d);
}
