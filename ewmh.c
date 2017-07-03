// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "ewmh.h"
#include <X11/Xatom.h>
#include <string.h>
#include <unistd.h>
#include "JBWMAtomIndex.h"
#include "JBWMClient.h"
#include "JBWMScreen.h"
#include "client.h"
#include "config.h"
#include "font.h"
#include "geometry.h"
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
#include "ewmh_atoms.c"
	XInternAtoms(d, jbwm_atom_names,
		JBWM_EWMH_ATOMS_COUNT, false, jbwm_ewmh);
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
static unsigned int get_window_list(Display * d, const uint8_t max_clients,
	Window * window_list)
{
	Window * wl, nil;
	unsigned int n;
	if (XQueryTree(d, DefaultRootWindow(d), &nil, &nil, &wl, &n)) {
		n = JB_MIN(n, max_clients); // limit to MAX_CLIENTS
		memcpy(window_list, wl, n * sizeof(Window));
		XFree(wl);
	}
	return n;
}
static Window * get_ordered_client_list(Display * d)
{
	enum {MAX_CLIENTS= 64};
	static Window window_list[MAX_CLIENTS];
	// get ordered list of all windows on default screen:
	const unsigned int n = get_window_list(d, MAX_CLIENTS, window_list);
	JBWM_LOG("get_ordered_client_list() n: %d", (int)n);
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
static void init_desktops(Display * d, struct JBWMScreen * s)
{
	const uint8_t id = s->id;
	static Window r;
	r = RootWindow(d, id);
	enum {INT=XA_CARDINAL, WIN=XA_WINDOW};
	struct JBWMSize sz = jbwm_get_display_size(d, s->id);
	wprop(d, r, JBWM_EWMH_DESKTOP_GEOMETRY, INT,
		(int32_t[]){sz.width, sz.height}, 2);
	wprop(d, r, JBWM_EWMH_CURRENT_DESKTOP, INT, &s->vdesk, 1);
	wprop(d, r, JBWM_EWMH_NUMBER_OF_DESKTOPS, INT,
		&(long){JBWM_MAX_DESKTOPS}, 1);
	wprop(d, r, JBWM_EWMH_DESKTOP_VIEWPORT, INT,
		&(long[]){0, 0}, 2);
	// Declared r static to keep scope
	wprop(d, r, JBWM_EWMH_VIRTUAL_ROOTS, WIN, &r, 1);
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
static Window init_supporting(Display * d, const Window r)
{
	Window w = XCreateSimpleWindow(d, r, 0, 0, 1, 1, 0, 0, 0);
	set_supporting(d, r, &w);
	set_supporting(d, w, &w);
	wprop(d, w, JBWM_EWMH_WM_PID, XA_CARDINAL, &(pid_t){getpid()}, 1);
	set_name(d, w);
	return w;
}
void jbwm_ewmh_init_screen(Display * d, struct JBWMScreen * s)
{
	if (!jbwm_ewmh[0])
		jbwm_ewmh_init(d);
	static Window r;
	r = RootWindow(d, s->id);
	wprop(d, r, JBWM_EWMH_SUPPORTED, XA_ATOM, jbwm_ewmh,
		JBWM_EWMH_ATOMS_COUNT);
	set_name(d, r);
	/* Set this to the root window until we have some clients.
	 * Declared r static so we don't lose scope.  */
	wprop(d, r, JBWM_EWMH_CLIENT_LIST, XA_WINDOW, &r, 1);
	init_desktops(d, s);
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
	wprop(c->display, c->parent, JBWM_EWMH_FRAME_EXTENTS,
		XA_CARDINAL, f, 4);
}
