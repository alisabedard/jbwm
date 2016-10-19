// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com>
#include "ewmh_state.h"
#include "client.h"
#include "ewmh.h"
#include "JBWMEnv.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "util.h"
#include <X11/Xatom.h>
// Remove specified atom from WM_STATE
void jbwm_ewmh_remove_state(const Window w, const Atom state)
{
	uint16_t n;
	Atom *a = jbwm_get_property(w, ewmh[WM_STATE], &n);
	if (!a)
		return;
	const uint16_t nitems = n;
	while (n--) // decrement here to prevent offset error
		if (a[n] == state)
			a[n] = 0;
	jbwm_set_property(w, ewmh[WM_STATE], XA_ATOM, a, nitems);
	XFree(a);
}
static bool ewmh_get_state(const Window w, const Atom state)
{
	uint16_t n;
	Atom *a = jbwm_get_property(w, ewmh[WM_STATE], &n);
	bool found = false;
	if (a) {
		while (n--) // prevent offset error
			if ((found = (a[n] == state)))
				break;
		XFree(a);
	}
	return found;
}
void jbwm_ewmh_add_state(const Window w, Atom state)
{
	XChangeProperty(jbwm.d, w, ewmh[WM_STATE],
		XA_ATOM, 32, PropModePrepend,
		(unsigned char *)&state, 1);
}
/*      Reference, per wm-spec:
  window  = the respective client window
  message_type = _NET_WM_STATE
  format = 32
  data.l[0] = the action, as listed below
        _NET_WM_STATE_REMOVE        0    remove/unset property
        _NET_WM_STATE_ADD           1    add/set property
        _NET_WM_STATE_TOGGLE        2    toggle property
  data.l[1] = first property to alter
  data.l[2] = second property to alter
  data.l[3] = source indication
  other data.l[] elements = 0 */
static void set_state(struct JBWMClient * restrict c,
	const bool add, const JBWMAtomIndex t)
{
	if (!c)
		return;
	switch(t) {
	case WM_STATE_FULLSCREEN:
		add?jbwm_set_fullscreen(c):jbwm_set_not_fullscreen(c);
		break;
	case WM_STATE_STICKY:
		c->opt.sticky=add;
		break;
	case WM_STATE_ABOVE:
		(add ? XRaiseWindow : XLowerWindow)(jbwm.d, c->parent);
		break;
	case WM_STATE_BELOW:
		(add ? XLowerWindow : XRaiseWindow)(jbwm.d, c->parent);
		break;
	case WM_STATE_HIDDEN:
		LOG("HIDDEN");
		break;
	case WM_STATE_MAXIMIZED_VERT:
		(add ? jbwm_set_vert : jbwm_set_not_vert)(c);
		break;
	case WM_STATE_MAXIMIZED_HORZ:
		(add ? jbwm_set_horz : jbwm_set_not_horz)(c);
		break;
	default:
		break;
	}
}
__attribute__((nonnull(1,3)))
static void check_state(XClientMessageEvent * e,	// event data
			const JBWMAtomIndex t,	// state to test
			struct JBWMClient *c)
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
		jbwm_ewmh_remove_state(e->window, state);
		break;
	case 1:	// add
		set_state(c, true, t);
		jbwm_ewmh_add_state(e->window, state);
		break;
	case 2:{	// toggle
			const bool add = !ewmh_get_state(e->window, state);
			set_state(c, add, t);
			(add ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
				(e->window, state);
		}
	}
}
__attribute__((nonnull(1,2)))
static void handle_wm_state_changes(XClientMessageEvent * restrict e,
	struct JBWMClient * restrict c)
{
	check_state(e, WM_STATE_ABOVE, c);
	check_state(e, WM_STATE_BELOW, c);
	check_state(e, WM_STATE_FULLSCREEN, c);
	check_state(e, WM_STATE_MAXIMIZED_HORZ, c);
	check_state(e, WM_STATE_MAXIMIZED_VERT, c);
	check_state(e, WM_STATE_STICKY, c);
}
static bool client_specific_message(XClientMessageEvent * restrict e,
	struct JBWMClient * restrict c, const Atom t)
{
	if (t == ewmh[WM_DESKTOP])
		jbwm_set_client_vdesk(c, e->data.l[0]);
	// If user moves window (client-side titlebars):
	else if (t == ewmh[WM_MOVERESIZE]) {
		XRaiseWindow(jbwm.d, c->parent);
		jbwm_drag(c, false);
	} else if (t == ewmh[WM_STATE])
		handle_wm_state_changes(e, c);
	else if (t == ewmh[ACTIVE_WINDOW])
		jbwm_select_client(c);
	else if (t == ewmh[CLOSE_WINDOW])
		jbwm_send_wm_delete(c);
	else
		  return false;
	return true;
}
static void handle_moveresize(XClientMessageEvent * restrict e)
{
	const uint8_t src = (e->data.l[0] >> 12) & 3;
	if (src != 2)
		return;
	const int vm = (e->data.l[0] >> 8) & 0x0f;
	XConfigureWindow(e->display, e->window,
		vm, &(XWindowChanges){
		.x = e->data.l[1], .y = e->data.l[2],
		.width = e->data.l[3],
		.height = e->data.l[4]});
}
void jbwm_ewmh_handle_client_message(XClientMessageEvent * restrict e,
	struct JBWMClient * restrict c)
{
	const Atom t = e->message_type;
#ifdef JBWM_EWMH_DEBUG
	dprintf(STDERR_FILENO, "----CLIENTMESSAGE----");
	jbwm_print_atom(t, __FILE__, __LINE__);
	jbwm_print_atom(e->data.l[0], __FILE__, __LINE__);
	jbwm_print_atom(e->data.l[1], __FILE__, __LINE__);
	jbwm_print_atom(e->data.l[2], __FILE__, __LINE__);
	jbwm_print_atom(e->data.l[3], __FILE__, __LINE__);
#endif//JBWM_EWMH_DEBUG
	struct JBWMScreen *s = c ? c->screen : jbwm.s;
	if(c && client_specific_message(e, c, t))
		  return;
	if (t == ewmh[CURRENT_DESKTOP])
		  jbwm_set_vdesk(s, e->data.l[0]);
	// If something else moves the window:
	else if (t == ewmh[MOVERESIZE_WINDOW])
		handle_moveresize(e);
}
