// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <stdbool.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include "atoms.h"
#include "client.h"
#include "client_t.h"
#include "events.h"
#include "ewmh.h"
#include "jbwmenv.h"
#include "log.h"
#include "new.h"
#include "screen.h"
#include "util.h"

EWMHEnvironment ewmh;

static char * atom_names [] = {
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
	"_NET_RESTACK_WINDOW",
	"_NET_REQUEST_FRAME_EXTENTS",
	"_NET_VIRTUAL_ROOTS",
	"_NET_CLIENT_LIST_STACKING",
	"_NET_WM_ALLOWED_ACTIONS",
	"_NET_WM_NAME",
	"_NET_WM_DESKTOP",
	"_NET_WM_MOVERESIZE",
	"_NET_WM_PID",
	"_NET_WM_MOVERESIZE_CANCEL",
	"_NET_WM_WINDOW_TYPE",
	"_NET_WM_STRUT_PARTIAL",
	"WM_CHANGE_STATE",
	"_NET_WM_USER_TIME",
	"_NET_WM_OPAQUE_REGION",
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
	"_NET_WM_STATE_DEMANDS_ATTENTION",
	"_NET_WM_STATE_FOCUSED",
	"_NET_WM_STATE_SKIP_PAGER"
};

void ewmh_init(void)
{
	ewmh.count = sizeof(atom_names) / sizeof(char *);
	LOG("atom_names: %d\n", ewmh.count);
	ewmh.atoms=malloc(ewmh.count*sizeof(Atom));
	XInternAtoms(jbwm.dpy, atom_names, ewmh.count, false, ewmh.atoms);
}

static inline void set_desktop_viewport(void)
{
	XPROP(jbwm.screens->root, ewmh.atoms[DESKTOP_VIEWPORT], XA_CARDINAL,
		(&(long[]){0, 0}), 2);
}

void ewmh_update_client_list()
{
#define MAX_CLIENTS 1024
	// Prevent data from disappearing after return.
	static Window wl[MAX_CLIENTS];
	register size_t count = 0;

	for (Client * i = jbwm.head; i && (count < MAX_CLIENTS); i = i->next)
		wl[count++] = i->window;

	XPROP(jbwm.screens->root, ewmh.atoms[CLIENT_LIST],
		XA_WINDOW, &wl, count);
	// FIXME: Does not correctly report stacking order.
	XPROP(jbwm.screens->root, ewmh.atoms[CLIENT_LIST_STACKING],
		XA_WINDOW, &wl, count);
}

// Remove specified atom from WM_STATE
void ewmh_remove_state(const Window w, const Atom state)
{
	unsigned long n;
	Atom *a = get_property(w, ewmh.atoms[WM_STATE], &n);

	if (a) {
		const unsigned long nitems = n;

		while (n--)
			if (a[n] == state)
				a[n] = 0;

		XPROP(w, ewmh.atoms[WM_STATE], XA_ATOM, &a, nitems);
		XFree(a);
	}
}

bool ewmh_get_state(const Window w, const Atom state)
{
	unsigned long n;
	Atom *a = get_property(w, ewmh.atoms[WM_STATE], &n);

	if (!a)
		return false;

	bool found = false;

	while (n--)
		if ((found = (a[n] == state)))
			break;

	XFree(a);
	return found;
}

void ewmh_add_state(const Window w, const Atom state)
{
	XChangeProperty(jbwm.dpy, w, ewmh.atoms[WM_STATE],
		XA_ATOM, 32, PropModePrepend,
		(unsigned char *)&state, 1);
}

static void set_number_of_desktops(const Window r)
{
	XPROP(r, ewmh.atoms[NUMBER_OF_DESKTOPS], XA_CARDINAL,
		&(long){DESKTOPS}, 1);
	set_desktop_viewport();
	XPROP(r, ewmh.atoms[VIRTUAL_ROOTS], XA_WINDOW, &r, 1);
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

static void check_state(XClientMessageEvent * e,	// event data
			const Atom state,	// state to test
			Client *c,
			// callback:
			void (*state_cb) (const bool, // true if add
			Client *))
{
	// 2 atoms can be set at once
	long * l = &e->data.l[0];
	const bool set = l[1] == (long)state || l[2] == (long)state;
	if(!set) return;
	switch (e->data.l[0]) {
	default:
	case 0:	// remove
		state_cb(false, c);
		ewmh_remove_state(e->window, state);
		break;

	case 1:	// add
		state_cb(true, c);
		ewmh_add_state(e->window, state);
		break;

	case 2:{	// toggle
			const bool add =
			    !ewmh_get_state(e->window, state);
			state_cb(false, c);

			if (add)
				  ewmh_add_state(e->window, state);
			else
				  ewmh_remove_state(e->window, state);
		}
	}
}

__attribute__((cold))
static void layer(const bool above, const Window w)
{
	if(above)
		  XRaiseWindow(jbwm.dpy, w);
	else
		  XLowerWindow(jbwm.dpy, w);
}

__attribute__((nonnull(2),cold))
static void above_cb(const bool add, Client * c)
{
	add?layer(true, c->parent):layer(false, c->parent);
}

__attribute__((nonnull(2),cold))
static void below_cb(const bool add, Client * c)
{
	add?layer(false, c->parent):layer(true, c->parent);
}

__attribute__((nonnull(2)))
static void fullscreen_cb(const bool add, Client * c)
{
	add?set_fullscreen(c):unset_fullscreen(c);
}

__attribute__((nonnull(2)))
static void max_h_cb(const bool add, Client * c)
{
	add?maximize_horz(c):restore_horz(c);
}

__attribute__((nonnull(2)))
static void
max_v_cb(const bool add, Client * c)
{
	add?maximize_vert(c):restore_vert(c);
}

__attribute__((nonnull(2),cold))
static void
stick_cb(const bool add, Client * c)
{
	if(add)
		  c->flags |= JB_STICKY;
	else
		  c->flags &= ~JB_STICKY;
}

__attribute__((nonnull(1,2)))
static void handle_wm_state_changes(XClientMessageEvent * restrict e,
	Client * restrict c)
{
	check_state(e, ewmh.atoms[WM_STATE_ABOVE], c, above_cb);
	check_state(e, ewmh.atoms[WM_STATE_BELOW], c, below_cb);
	check_state(e, ewmh.atoms[WM_STATE_FULLSCREEN], c, fullscreen_cb);
	check_state(e, ewmh.atoms[WM_STATE_MAXIMIZED_HORZ], c, max_h_cb);
	check_state(e, ewmh.atoms[WM_STATE_MAXIMIZED_VERT], c, max_v_cb);
	check_state(e, ewmh.atoms[WM_STATE_STICKY], c, stick_cb);
}

__attribute__((nonnull))
void ewmh_client_message(XClientMessageEvent * e)
{
	const Atom t = e->message_type;
#ifdef DEBUG
	puts("----CLIENTMESSAGE----");
	print_atom(t, __LINE__);
	print_atom(e->data.l[0], __LINE__);
	print_atom(e->data.l[1], __LINE__);
	print_atom(e->data.l[2], __LINE__);
	print_atom(e->data.l[3], __LINE__);
#endif//DEBUG
	Client *c = find_client(e->window);
	ScreenInfo *s = c ? c->screen : jbwm.screens;
	const long val = e->data.l[0];

	if (t == ewmh.atoms[CURRENT_DESKTOP])
		switch_vdesk(s, val);
	if (t == ewmh.atoms[WM_DESKTOP] && c)
		client_to_vdesk(c, val);
	else if (t == ewmh.atoms[DESKTOP_VIEWPORT])
		set_desktop_viewport();
	else if (t == ewmh.atoms[NUMBER_OF_DESKTOPS])
		set_number_of_desktops(s->root);
	else if (t == ewmh.atoms[ACTIVE_WINDOW] && val == 2 && c)
		select_client(c);
	else if (t == ewmh.atoms[CLOSE_WINDOW] && e->data.l[1] == 2 && c)
		send_wm_delete(c);
	// If something else moves the window:
	else if (t == ewmh.atoms[MOVERESIZE_WINDOW]) {
		const uint8_t src = (val >> 12) & 3;
		if (src == 2) {
			const int vm = (val >> 8) & 0x0f;
			//const int grav = val & 0xff;
			XConfigureWindow(e->display, e->window,
				vm, &(XWindowChanges){
				.x = e->data.l[1], .y = e->data.l[2],
				.width = e->data.l[3],
				.height = e->data.l[4]});
		}
	}
	// If user moves window (client-side titlebars):
	else if (t == ewmh.atoms[WM_MOVERESIZE] && c) {
		XRaiseWindow(jbwm.dpy, c->parent);
		drag(c);
	} else if (t == ewmh.atoms[WM_STATE] && c)
		handle_wm_state_changes(e, c);
	else if (t == ewmh.atoms[WM_CHANGE_STATE] && c) {
		if (val == 3)	// Minimize (lower)
			XLowerWindow(jbwm.dpy, c->parent);
	}
}

void set_ewmh_allowed_actions(const Window w)
{
	const Atom a[] = {
		ewmh.atoms[WM_ALLOWED_ACTIONS],
		ewmh.atoms[WM_ACTION_MOVE],
		ewmh.atoms[WM_ACTION_RESIZE], ewmh.atoms[WM_ACTION_CLOSE],
		ewmh.atoms[WM_ACTION_SHADE],
		ewmh.atoms[WM_ACTION_FULLSCREEN],
		ewmh.atoms[WM_ACTION_CHANGE_DESKTOP],
		ewmh.atoms[WM_ACTION_ABOVE],
		ewmh.atoms[WM_ACTION_BELOW],
		ewmh.atoms[WM_ACTION_MAXIMIZE_HORZ],
		ewmh.atoms[WM_ACTION_MAXIMIZE_VERT]
	};
	XPROP(w, a[0], XA_ATOM, &a, sizeof(a) / sizeof(Atom));
}

__attribute__((nonnull(1)))
static void init_desktops(ScreenInfo * restrict s)
{
	const Window r = s->root;
	const unsigned long workarea[4] = { 0, 0, s->size.w, s->size.h };
	XPROP(r, ewmh.atoms[DESKTOP_VIEWPORT], XA_CARDINAL, &workarea[0], 2);
	XPROP(r, ewmh.atoms[DESKTOP_GEOMETRY], XA_CARDINAL, &workarea[2], 2);
	XPROP(r, ewmh.atoms[CURRENT_DESKTOP], XA_CARDINAL, &s->vdesk, 1);
	static const unsigned char n = DESKTOPS;
	XPROP(r, ewmh.atoms[NUMBER_OF_DESKTOPS], XA_CARDINAL, &n, 1);
}

__attribute__((nonnull(1)))
static void init_supporting(ScreenInfo * restrict s)
{
	s->supporting = XCreateSimpleWindow(jbwm.dpy, s->root,
		0, 0, 1, 1, 0, 0, 0);
	XPROP(s->root, ewmh.atoms[SUPPORTING_WM_CHECK],
		XA_WINDOW, &(s->supporting), 1);
	XPROP(s->supporting, ewmh.atoms[SUPPORTING_WM_CHECK],
		XA_WINDOW, &(s->supporting), 1);
	XPROP(s->supporting, ewmh.atoms[WM_NAME], XA_STRING, "jbwm", 4);
	XPROP(s->supporting, ewmh.atoms[WM_PID], XA_CARDINAL,
		&(pid_t){getpid()}, 1);
}

__attribute__((nonnull(1)))
void setup_ewmh_for_screen(ScreenInfo * restrict s)
{
	XPROP(s->root, ewmh.atoms[SUPPORTED], XA_ATOM, ewmh.atoms, ewmh.count);
	XPROP(s->root, ewmh.atoms[WM_NAME], XA_STRING, "jbwm", 4);
	// Set this to the root window until we have some clients.
	XPROP(s->root, ewmh.atoms[CLIENT_LIST], XA_WINDOW, &s->root, 1);
	init_desktops(s);
	init_supporting(s);
}

