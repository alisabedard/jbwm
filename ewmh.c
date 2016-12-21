// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "ewmh.h"
#include <string.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include "JBWMClient.h"
#include "JBWMDefaults.h"
#include "JBWMScreen.h"
#include "client.h"
#include "config.h"
#include "display.h"
#include "font.h"
#include "log.h"
#include "macros.h"
#include "util.h"
static Atom jbwm_ewmh[JBWM_EWMH_ATOMS_COUNT];
Atom jbwm_ewmh_get_atom(const uint8_t index)
{
	return jbwm_ewmh[index];
}
static void jbwm_ewmh_init(Display * d)
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
static inline void wprop(Display * d, const Window win,
	const enum JBWMAtomIndex i, const Atom type,
	void * data, const uint16_t size)
{
	jbwm_set_property(d, win, jbwm_ewmh[i], type, data, size);
}
static Window * get_mixed_client_list(Display * d)
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
static Window * get_ordered_client_list(Display * d)
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
void jbwm_ewmh_update_client_list(Display * d)
{
	get_mixed_client_list(d);
	get_ordered_client_list(d);
}
void jbwm_ewmh_set_allowed_actions(Display * d,
	const Window w)
{
#define EWMHWM(a) jbwm_ewmh[JBWM_EWMH_WM_##a]
#define ACTION(a) EWMHWM(ACTION_##a)
	Atom a[] = {
		EWMHWM(ALLOWED_ACTIONS),
		ACTION(MOVE), ACTION(RESIZE), ACTION(CLOSE), ACTION(SHADE),
		ACTION(FULLSCREEN), ACTION(CHANGE_DESKTOP), ACTION(ABOVE),
		ACTION(BELOW), ACTION(MAXIMIZE_HORZ), ACTION(MAXIMIZE_VERT)
	};
	jbwm_set_property(d, w, a[0], XA_ATOM, &a, sizeof(a) / sizeof(Atom));
}
static void init_desktops(struct JBWMScreen * s)
{
	Display * d = jbwm_get_display();
	const Window r = s->root;
	enum {INT=XA_CARDINAL, WIN=XA_WINDOW};
	wprop(d, r, JBWM_EWMH_DESKTOP_GEOMETRY, INT, &s->size, 2);
	wprop(d, r, JBWM_EWMH_CURRENT_DESKTOP, INT, &s->vdesk, 1);
	wprop(d, r, JBWM_EWMH_NUMBER_OF_DESKTOPS, INT,
		&(long){JBWM_MAX_DESKTOPS}, 1);
	wprop(d, r, JBWM_EWMH_DESKTOP_VIEWPORT, INT,
		&(long[]){0, 0}, 2);
	/* Pass the address of the field within s to keep scope.  */
	wprop(d, r, JBWM_EWMH_VIRTUAL_ROOTS, WIN, &s->root, 1);
}
static void set_name(Display * d, const Window w)
{
	wprop(d, w, JBWM_EWMH_WM_NAME, XA_STRING, JBWM_NAME,
		sizeof(JBWM_NAME));
}
static void set_supporting(Display * d, const Window w,
	Window * s)
{
	wprop(d, w, JBWM_EWMH_SUPPORTING_WM_CHECK, XA_WINDOW, s, 1);
}
static Window init_supporting(Display * d,
	const Window r)
{
	Window w = XCreateSimpleWindow(d, r, 0, 0, 1, 1, 0, 0, 0);
	set_supporting(d, r, &w);
	set_supporting(d, w, &w);
	wprop(d, w, JBWM_EWMH_WM_PID, XA_CARDINAL, &(pid_t){getpid()}, 1);
	set_name(d, w);
	return w;
}
void jbwm_ewmh_init_screen(struct JBWMScreen * s)
{
	Display * d = jbwm_get_display();
	if (!jbwm_ewmh[0])
		jbwm_ewmh_init(d);
	Window r = s->root;
	wprop(d, r, JBWM_EWMH_SUPPORTED, XA_ATOM, jbwm_ewmh,
		JBWM_EWMH_ATOMS_COUNT);
	set_name(d, r);
	/* Set this to the root window until we have some clients.
	 * Use the address of the r field within s, not &r, so we
	 * don't lose scope.  */
	wprop(d, r, JBWM_EWMH_CLIENT_LIST, XA_WINDOW, &s->root, 1);
	init_desktops(s);
	s->supporting = init_supporting(d, r);
}
// Required by wm-spec:
void jbwm_set_frame_extents(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_set_frame_extents()");
	// Fields: left, right, top, bottom
	static uint32_t f[4];
	const uint8_t b = c->border;
	f[0] = f[1] = f[2] = f[3] = b;
	if (!c->opt.no_title_bar)
		f[2] += jbwm_get_font_height();
	wprop(jbwm_get_display(), c->parent,
		JBWM_EWMH_FRAME_EXTENTS, XA_CARDINAL, f, 4);
}
