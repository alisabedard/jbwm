// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "ewmh.h"

#include "client.h"
#include "jbwmenv.h"
#include "log.h"
#include "screen.h"
#include "util.h"

#include <unistd.h>
#include <stdlib.h>
#include <X11/Xatom.h>

//EWMHEnvironment ewmh;
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
	"WM_CHANGE_STATE",
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
	XInternAtoms(jbwm.dpy, atom_names, EWMH_ATOMS_COUNT, false, ewmh);
}

void ewmh_update_client_list(void)
{
	enum { MAX_CLIENTS = 64 };
	// Prevent data from disappearing after return.
	static Window wl[MAX_CLIENTS];
	register size_t count = 0;

	for (Client * i = jbwm.head; i && (count < MAX_CLIENTS); i = i->next)
		wl[count++] = i->window;

	XPROP(jbwm.screens->root, ewmh[CLIENT_LIST],
		XA_WINDOW, &wl, count);
	// FIXME: Does not correctly report stacking order.
	XPROP(jbwm.screens->root, ewmh[CLIENT_LIST_STACKING],
		XA_WINDOW, &wl, count);
}

// Remove specified atom from WM_STATE
void ewmh_remove_state(const Window w, const Atom state)
{
	unsigned long n;
	Atom *a = get_property(w, ewmh[WM_STATE], &n);

	if (a) {
		const unsigned long nitems = n;

		while (n--)
			if (a[n] == state)
				a[n] = 0;

		XPROP(w, ewmh[WM_STATE], XA_ATOM, &a, nitems);
		XFree(a);
	}
}

static bool ewmh_get_state(const Window w, const Atom state)
{
	unsigned long n;
	Atom *a = get_property(w, ewmh[WM_STATE], &n);

	bool found = false;
	if (a) {
		while (n--)
			if ((found = (a[n] == state)))
				break;
		XFree(a);
	}
	return found;
}

void ewmh_add_state(const Window w, Atom state)
{
	XChangeProperty(jbwm.dpy, w, ewmh[WM_STATE],
		XA_ATOM, 32, PropModePrepend,
		(unsigned char *)&state, 1);
}

static void set_root_vdesk(const Window r)
{
	XPROP(r, ewmh[NUMBER_OF_DESKTOPS], XA_CARDINAL,
		&(long){DESKTOPS}, 1);
	XPROP(r, ewmh[DESKTOP_VIEWPORT], XA_CARDINAL,
		(&(long[]){0, 0}), 2);
	XPROP(r, ewmh[VIRTUAL_ROOTS], XA_WINDOW, &r, 1);
}

/*      Reference, per wm-spec:
  window  = the respective client window
  message_type = _NET_WM_STATE
  format = 32
  data.l[0] = the action, as listed below
  data.l[1] = first property to alter
  data.l[2] = second property to alter
  data.l[3] = source indication:
        _NET_WM_STATE_REMOVE        0    remove/unset property
        _NET_WM_STATE_ADD           1    add/set property
        _NET_WM_STATE_TOGGLE        2    toggle property
  other data.l[] elements = 0 */

__attribute__((nonnull(1)))
static void set_state(Client * restrict c,
	const bool add, const AtomIndex t)
{
	switch(t) {
	case WM_STATE_FULLSCREEN:
		add?set_fullscreen(c):unset_fullscreen(c);
		break;
	case WM_STATE_STICKY:
		c->opt.sticky=add;
		break;
	case WM_STATE_ABOVE:
		if(add) XRaiseWindow(jbwm.dpy, c->parent);
		else XLowerWindow(jbwm.dpy, c->parent);
		break;
	case WM_STATE_BELOW:
		if(add) XLowerWindow(jbwm.dpy, c->parent);
		else XRaiseWindow(jbwm.dpy, c->parent);
		break;
	case WM_STATE_MAXIMIZED_VERT:
		add?maximize_vert(c):restore_vert(c);
		break;
	case WM_STATE_MAXIMIZED_HORZ:
		add?maximize_horz(c):restore_horz(c);
		break;
	default:
		break;
	}
}

__attribute__((nonnull(1,3)))
static void check_state(XClientMessageEvent * e,	// event data
			const AtomIndex t,	// state to test
			Client *c)
{
	const Atom state = ewmh[t];
	// 2 atoms can be set at once
	long * l = &e->data.l[0];
	const bool set = l[1] == (long)state || l[2] == (long)state;
	if(!set) return;
	switch (e->data.l[0]) {
	default:
	case 0:	// remove
		set_state(c, false, t);
		ewmh_remove_state(e->window, state);
		break;

	case 1:	// add
		set_state(c, true, t);
		ewmh_add_state(e->window, state);
		break;

	case 2:{	// toggle
			const bool add = !ewmh_get_state(e->window, state);
			set_state(c, add, t);
			if (add)
				  ewmh_add_state(e->window, state);
			else
				  ewmh_remove_state(e->window, state);
		}
	}
}

__attribute__((nonnull(1,2)))
static void handle_wm_state_changes(XClientMessageEvent * restrict e,
	Client * restrict c)
{
	check_state(e, WM_STATE_ABOVE, c);
	check_state(e, WM_STATE_BELOW, c);
	check_state(e, WM_STATE_FULLSCREEN, c);
	check_state(e, WM_STATE_MAXIMIZED_HORZ, c);
	check_state(e, WM_STATE_MAXIMIZED_VERT, c);
	check_state(e, WM_STATE_STICKY, c);
}

void ewmh_client_message(XClientMessageEvent * restrict e,
	Client * restrict c)
{
	const Atom t = e->message_type;
#ifdef DEBUG
	fprintf(stderr, "----CLIENTMESSAGE----");
	print_atom(t, __LINE__);
	print_atom(e->data.l[0], __LINE__);
	print_atom(e->data.l[1], __LINE__);
	print_atom(e->data.l[2], __LINE__);
	print_atom(e->data.l[3], __LINE__);
#endif//DEBUG
	ScreenInfo *s = c ? c->screen : jbwm.screens;
	const long val = e->data.l[0];
	if (t == ewmh[CURRENT_DESKTOP])
		switch_vdesk(s, val);
	else if (t == ewmh[WM_DESKTOP] && c)
		client_to_vdesk(c, val);
	else if ((t == ewmh[NUMBER_OF_DESKTOPS])
		|| (t == ewmh[DESKTOP_VIEWPORT]))
		set_root_vdesk(s->root);
	else if (t == ewmh[ACTIVE_WINDOW] && val == 2 && c)
		select_client(c);
	else if (t == ewmh[CLOSE_WINDOW] && e->data.l[1] == 2 && c)
		send_wm_delete(c);
	// If something else moves the window:
	else if (t == ewmh[MOVERESIZE_WINDOW]) {
		const uint8_t src = (val >> 12) & 3;
		if (src == 2) {
			const int vm = (val >> 8) & 0x0f;
			XConfigureWindow(e->display, e->window,
				vm, &(XWindowChanges){
				.x = e->data.l[1], .y = e->data.l[2],
				.width = e->data.l[3],
				.height = e->data.l[4]});
		}
	}
	// If user moves window (client-side titlebars):
	else if (t == ewmh[WM_MOVERESIZE] && c) {
		XRaiseWindow(jbwm.dpy, c->parent);
		drag(c);
	} else if (t == ewmh[WM_STATE] && c)
		handle_wm_state_changes(e, c);
	else if (t == ewmh[WM_CHANGE_STATE] && c) {
		if (val == 3)	// Minimize (lower)
			XLowerWindow(jbwm.dpy, c->parent);
	}
}

void set_ewmh_allowed_actions(const Window w)
{
	const Atom a[] = {
		ewmh[WM_ALLOWED_ACTIONS],
		ewmh[WM_ACTION_MOVE],
		ewmh[WM_ACTION_RESIZE], ewmh[WM_ACTION_CLOSE],
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

static void init_desktops(const Window r, const Dim sz, const uint8_t vdesk)
{
	XPROP(r, ewmh[DESKTOP_GEOMETRY], XA_CARDINAL, &sz, 2);
	XPROP(r, ewmh[CURRENT_DESKTOP], XA_CARDINAL, &vdesk, 1);
	set_root_vdesk(r);
}

static Window init_supporting(const Window r)
{
	Window w = XCreateSimpleWindow(jbwm.dpy, r, 0, 0, 1, 1, 0, 0, 0);
	XPROP(r, ewmh[SUPPORTING_WM_CHECK], XA_WINDOW, &w, 1);
	XPROP(w, ewmh[SUPPORTING_WM_CHECK], XA_WINDOW, &w, 1);
	XPROP(w, ewmh[WM_NAME], XA_STRING, "jbwm", 4);
	XPROP(w, ewmh[WM_PID], XA_CARDINAL, &(pid_t){getpid()}, 1);
	return w;
}

__attribute__((nonnull(1)))
void setup_ewmh_for_screen(ScreenInfo * restrict s)
{
	const Window r = s->root;
	XPROP(r, ewmh[SUPPORTED], XA_ATOM, ewmh, EWMH_ATOMS_COUNT);
	XPROP(r, ewmh[WM_NAME], XA_STRING, "jbwm", 4);
	// Set this to the root window until we have some clients.
	XPROP(r, ewmh[CLIENT_LIST], XA_WINDOW, &r, 1);
	init_desktops(r, s->size, s->vdesk);
	s->supporting=init_supporting(r);
}

