// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com>
#include "ewmh_state.h"
#include <X11/Xatom.h>
#include "client.h"
#include "ewmh.h"
#include "jbwm.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "select.h"
#include "util.h"
#include "wm_state.h"
#define JBWM_DEBUG_EWMH_STATE
#ifndef JBWM_DEBUG_EWMH_STATE
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!JBWM_DEBUG_EWMH_STATE
// Remove specified atom from WM_STATE
void jbwm_ewmh_remove_state(Display * restrict d,
	const jbwm_window_t w, const jbwm_atom_t state)
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
	const jbwm_window_t w, const jbwm_atom_t state)
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
void jbwm_ewmh_add_state(Display * restrict d, const jbwm_window_t w,
	jbwm_atom_t state)
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
	bool add, const enum JBWMAtomIndex t)
{
	JBWM_LOG("set_state(c, add: %s, t: %d)", add ? "true" : "false",
		(int) t);
	jbwm_print_atom(c->d, jbwm_ewmh_get_atom(t), __FILE__, __LINE__);
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
		add = !add; // fall through
	case JBWM_EWMH_WM_STATE_BELOW:
		(add ? XRaiseWindow : XLowerWindow)(c->d, c->parent);
		break;
	case JBWM_EWMH_WM_STATE_HIDDEN:
		(add ? jbwm_hide_client : jbwm_restore_client)(c);
		break;
	case JBWM_EWMH_WM_STATE_MAXIMIZED_VERT:
		(add ? jbwm_set_vert : jbwm_set_not_vert)(c);
		break;
	case JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ:
		(add ? jbwm_set_horz : jbwm_set_not_horz)(c);
		break;
	default:
		JBWM_LOG("\tWARNING:  Unhandled state");
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
	jbwm_print_atom(c->d, t, __FILE__, __LINE__);
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
	/* per wm-spec: fields come from e->data.l
	   l[0]: gravity and flags, l[1]: x, l[2]: y, l[3]: w, l[4]: h
		l[0]: bits 0-7 idicate gravity,
		l[0]: bits 8-11 use x, y, width, height, respectively
		l[0]: bits 12-15 indicate the source,
	*/
	enum {
		SRC_SHIFT = 12, SRC_MASK = 3, VM_SHIFT = 8, VM_MASK = 0xf,
		USER_ACTION = 2
	};
	const long * restrict l = e->data.l;
	const uint8_t src = (l[0] >> SRC_SHIFT) & SRC_MASK;
	if (src != USER_ACTION)
		return;
	const uint32_t vm = (l[0] >> VM_SHIFT) & VM_MASK;
	XConfigureWindow(e->display, e->window, vm, &(XWindowChanges){
		.x = l[1], .y = l[2], .width = l[3], .height = l[4]});
	// use bit width truncation to mask for gravity:
	const uint8_t win_gravity = l[0];
	XChangeWindowAttributes(e->display, e->window, CWWinGravity,
		&(XSetWindowAttributes){.win_gravity = win_gravity});
}
#if defined(JBWM_DEBUG_EWMH_STATE) && defined(DEBUG)
static void debug_client_message(XClientMessageEvent * restrict e)
{
	Display * d = e->display;
	const long * l = e->data.l;
	JBWM_LOG("jbwm_ewmh_client_message()");
	jbwm_print_atom(d, e->message_type, __FILE__, __LINE__);
	JBWM_LOG("\t\tl[0: %ld, 1: %ld, 2: %ld, 3: %ld, 4: %ld]",
		l[0], l[1], l[2], l[3], l[4]);
}
#else//!JBWM_DEBUG_EWMH_STATE||!DEBUG
#define debug_client_message(e)
#endif//JBWM_DEBUG_EWMH_STATE&&DEBUG
void jbwm_ewmh_handle_client_message(XClientMessageEvent * restrict e,
	struct JBWMClient * restrict c)
{
	const jbwm_atom_t t = e->message_type;
	debug_client_message(e);
	if(c && client_specific_message(e, c, t))
		  return;
	enum {
		DESK = JBWM_EWMH_CURRENT_DESKTOP,
		MR_WIN = JBWM_EWMH_MOVERESIZE_WINDOW
	};
	if (t == jbwm_ewmh_get_atom(DESK)) {
		const uint8_t i = c ? c->screen : 0;
		jbwm_set_vdesk(jbwm_get_screens() + i, e->data.l[0]);
	} else if (t == jbwm_ewmh_get_atom(MR_WIN)) {
		// If something else moves the window:
		handle_moveresize(e);
	}
}
