// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com>
#include "ewmh_state.h"
#include "client.h"
#include "ewmh.h"
#include "jbwm.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "util.h"
#include <X11/Xatom.h>
//#define JBWM_DEBUG_EWMH_STATE
#ifndef JBWM_DEBUG_EWMH_STATE
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!JBWM_DEBUG_EWMH_STATE
// Remove specified atom from WM_STATE
void jbwm_ewmh_remove_state(Display * restrict d,
	const Window w, const Atom state)
{
	uint16_t n;
	const jbwm_atom_t ws = jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE);
	Atom *a = jbwm_get_property(d, w, ws, &n);
	if (!a)
		return;
	const uint16_t nitems = n;
	while (n--) // decrement here to prevent offset error
		if (a[n] == state)
			a[n] = 0;
	jbwm_set_property(d, w, ws, XA_ATOM, a, nitems);
	XFree(a);
}
static bool ewmh_get_state(Display * restrict d,
	const Window w, const Atom state)
{
	uint16_t n;
	Atom *a = jbwm_get_property(d, w,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE), &n);
	bool found = false;
	if (a) {
		while (n--) // prevent offset error
			if ((found = (a[n] == state)))
				break;
		XFree(a);
	}
	return found;
}
void jbwm_ewmh_add_state(Display * d, const Window w, Atom state)
{
	XChangeProperty(d, w, jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE),
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
	const bool add, const enum JBWMAtomIndex t)
{
	if (!c)
		return;
	switch(t) {
	case JBWM_EWMH_WM_STATE_FULLSCREEN:
		(add?jbwm_set_fullscreen:jbwm_set_not_fullscreen)(c);
		break;
	case JBWM_EWMH_WM_STATE_STICKY:
		c->opt.sticky=add;
		break;
	case JBWM_EWMH_WM_STATE_ABOVE:
		(add ? XRaiseWindow : XLowerWindow)(c->d, c->parent);
		break;
	case JBWM_EWMH_WM_STATE_BELOW:
		(add ? XLowerWindow : XRaiseWindow)(c->d, c->parent);
		break;
	case JBWM_EWMH_WM_STATE_HIDDEN:
		JBWM_LOG("HIDDEN");
		break;
	case JBWM_EWMH_WM_STATE_MAXIMIZED_VERT:
		(add ? jbwm_set_vert : jbwm_set_not_vert)(c);
		break;
	case JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ:
		(add ? jbwm_set_horz : jbwm_set_not_horz)(c);
		break;
	default:
		break;
	}
}
__attribute__((nonnull))
static void check_state(XClientMessageEvent * e,	// event data
			const enum JBWMAtomIndex t,	// state to test
			struct JBWMClient * c)
{
	const Atom state = jbwm_ewmh_get_atom(t);
	// 2 atoms can be set at once
	long * l = &e->data.l[0];
	const bool set = l[1] == (long)state || l[2] == (long)state;
	if(!set)
		return;
	Display * restrict d = e->display;
	switch (e->data.l[0]) {
	default:
	case 0:	// remove
		set_state(c, false, t);
		jbwm_ewmh_remove_state(d, e->window, state);
		break;
	case 1:	// add
		set_state(c, true, t);
		jbwm_ewmh_add_state(d, e->window, state);
		break;
	case 2: { // toggle
			const bool add = !ewmh_get_state(e->display,
				e->window, state);
			set_state(c, add, t);
			(add ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
				(d, e->window, state);
		}
	}
}
__attribute__((nonnull))
static void handle_wm_state_changes(XClientMessageEvent * restrict e,
	struct JBWMClient * restrict c)
{
	check_state(e, JBWM_EWMH_WM_STATE_ABOVE, c);
	check_state(e, JBWM_EWMH_WM_STATE_BELOW, c);
	check_state(e, JBWM_EWMH_WM_STATE_FULLSCREEN, c);
	check_state(e, JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ, c);
	check_state(e, JBWM_EWMH_WM_STATE_MAXIMIZED_VERT, c);
	check_state(e, JBWM_EWMH_WM_STATE_STICKY, c);
}
static bool client_specific_message(XClientMessageEvent * restrict e,
	struct JBWMClient * restrict c, const Atom t)
{
	if (t == jbwm_ewmh_get_atom(JBWM_EWMH_WM_DESKTOP))
		jbwm_set_client_vdesk(c, e->data.l[0]);
	// If user moves window (client-side title bars):
	else if (t == jbwm_ewmh_get_atom(JBWM_EWMH_WM_MOVERESIZE)) {
		XRaiseWindow(c->d, c->parent);
		jbwm_drag(c, false);
	} else if (t == jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE))
		handle_wm_state_changes(e, c);
	else if (t == jbwm_ewmh_get_atom(JBWM_EWMH_ACTIVE_WINDOW))
		jbwm_select_client(c);
	else if (t == jbwm_ewmh_get_atom(JBWM_EWMH_CLOSE_WINDOW))
		jbwm_send_wm_delete(c);
	else
		  return false;
	return true;
}
static void handle_moveresize(XClientMessageEvent * restrict e)
{
	enum {SRC_SHIFT = 12, SRC_MASK = 3, VM_SHIFT = 8, VM_MASK = 0xf};
	const uint8_t src = (e->data.l[0] >> SRC_SHIFT) & SRC_MASK;
	if (src != 2)
		return;
	const uint32_t vm = (e->data.l[0] >> VM_SHIFT) & VM_MASK;
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
#ifdef JBWM_DEBUG_EWMH_STATE
	dprintf(STDERR_FILENO, "----CLIENTMESSAGE----");
	jbwm_print_atom(t, __FILE__, __LINE__);
	jbwm_print_atom(e->data.l[0], __FILE__, __LINE__);
	jbwm_print_atom(e->data.l[1], __FILE__, __LINE__);
	jbwm_print_atom(e->data.l[2], __FILE__, __LINE__);
	jbwm_print_atom(e->data.l[3], __FILE__, __LINE__);
#endif//JBWM_DEBUG_EWMH_STATE
	if(c && client_specific_message(e, c, t))
		  return;
	if (t == jbwm_ewmh_get_atom(JBWM_EWMH_CURRENT_DESKTOP))
		  jbwm_set_vdesk(e->display,
			  &jbwm_get_screens()[c ? c->screen : 0],
			  e->data.l[0]);
	else if (t == jbwm_ewmh_get_atom(JBWM_EWMH_MOVERESIZE_WINDOW))
		// If something else moves the window:
		handle_moveresize(e);
}
