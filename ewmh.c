// jbwm - Minimalist jbwm_window_t Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "ewmh.h"

#include "client.h"
#include "jbwmenv.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <X11/Xatom.h>

Atom ewmh[EWMH_ATOMS_COUNT];

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

void ewmh_init(void)
{
	LOG("atom_names: %d\n", EWMH_ATOMS_COUNT);
	XInternAtoms(jbwm.d, atom_names, EWMH_ATOMS_COUNT, false, ewmh);
}

static uint16_t get_client_count(void)
{
	uint16_t j = 0;
	for (Client * i = jbwm.head; i; i = i->next, ++j);
	return j;
}

void ewmh_update_client_list(void)
{
	jbwm_window_t wl[get_client_count()];
	size_t wl_sz = 0;
	for (Client * i = jbwm.head; i; i = i->next, ++wl_sz)
		wl[wl_sz] = i->window;
	XPROP(jbwm.s->root, ewmh[CLIENT_LIST],
		XA_WINDOW, &wl, wl_sz);
	// FIXME: Does not correctly report stacking order.
	XPROP(jbwm.s->root, ewmh[CLIENT_LIST_STACKING],
		XA_WINDOW, &wl, wl_sz);
}
static void set_root_vdesk(const jbwm_window_t r)
{
	XPROP(r, ewmh[NUMBER_OF_DESKTOPS], XA_CARDINAL,
		&(long){DESKTOPS}, 1);
	XPROP(r, ewmh[DESKTOP_VIEWPORT], XA_CARDINAL,
		(&(long[]){0, 0}), 2);
	XPROP(r, ewmh[VIRTUAL_ROOTS], XA_WINDOW, &r, 1);
}


void set_ewmh_allowed_actions(const jbwm_window_t w)
{
	const Atom a[] = {
		ewmh[WM_ALLOWED_ACTIONS],
		ewmh[WM_ACTION_MOVE],
		ewmh[WM_ACTION_RESIZE],
		ewmh[WM_ACTION_CLOSE],
		ewmh[WM_ACTION_SHADE],
		ewmh[WM_ACTION_FULLSCREEN],
		ewmh[WM_ACTION_CHANGE_DESKTOP],
		ewmh[WM_ACTION_ABOVE],
		ewmh[WM_ACTION_BELOW],
		ewmh[WM_ACTION_MAXIMIZE_HORZ],
		ewmh[WM_ACTION_MAXIMIZE_VERT]
	};
	XPROP(w, a[0], XA_ATOM, &a, sizeof(a) / sizeof(Atom));
}

static void init_desktops(struct ScreenInfo * restrict s)
{
	XPROP(s->r, ewmh[DESKTOP_GEOMETRY], XA_CARDINAL, &s->size, 2);
	XPROP(s->r, ewmh[CURRENT_DESKTOP], XA_CARDINAL, &s->vdesk, 1);
	set_root_vdesk(s->r);
}

static jbwm_window_t init_supporting(const jbwm_window_t r)
{
	jbwm_window_t w = XCreateSimpleWindow(jbwm.d, r, 0, 0, 1, 1, 0, 0, 0);
	XPROP(r, ewmh[SUPPORTING_WM_CHECK], XA_WINDOW, &w, 1);
	XPROP(w, ewmh[SUPPORTING_WM_CHECK], XA_WINDOW, &w, 1);
	XPROP(w, ewmh[WM_NAME], XA_STRING, "jbwm", 4);
	XPROP(w, ewmh[WM_PID], XA_CARDINAL, &(pid_t){getpid()}, 1);
	return w;
}

__attribute__((nonnull(1)))
void setup_ewmh_for_screen(ScreenInfo * restrict s)
{
	const jbwm_window_t r = s->root;
	XPROP(r, ewmh[SUPPORTED], XA_ATOM, ewmh, EWMH_ATOMS_COUNT);
	XPROP(r, ewmh[WM_NAME], XA_STRING, "jbwm", 4);
	// Set this to the root window until we have some clients.
	XPROP(r, ewmh[CLIENT_LIST], XA_WINDOW, &r, 1);
	init_desktops(s);
	s->supporting = init_supporting(r);
}

