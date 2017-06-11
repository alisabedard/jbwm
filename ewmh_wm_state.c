// Copyright 2017, Jeffrey E. Bedard
#include "ewmh_wm_state.h"
#include "JBWMAtomIndex.h"
#include "JBWMClient.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "log.h"
#include "max.h"
#include "util.h"
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
	jbwm_print_atom(d, jbwm_ewmh_get_atom(t), __FILE__, __LINE__);
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
		(add ? XRaiseWindow : XLowerWindow)(c->display, c->parent);
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
			struct JBWMClient * restrict c)
{
	const Atom state = jbwm_ewmh_get_atom(t);
	// 2 atoms can be set at once
	long * l = &e->data.l[0];
	const bool set = l[1] == (long)state || l[2] == (long)state;
	if(!set)
		return;
	Display * d = e->display;
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
			const bool add = !jbwm_ewmh_get_state(e->display,
				e->window, state);
			set_state(c, add, t);
			(add ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
				(d, e->window, state);
		}
	}
}
void jbwm_ewmh_handle_wm_state_changes(XClientMessageEvent * e,
	struct JBWMClient * restrict c)
{
	check_state(e, JBWM_EWMH_WM_STATE_ABOVE, c);
	check_state(e, JBWM_EWMH_WM_STATE_BELOW, c);
	check_state(e, JBWM_EWMH_WM_STATE_FULLSCREEN, c);
	check_state(e, JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ, c);
	check_state(e, JBWM_EWMH_WM_STATE_MAXIMIZED_VERT, c);
	check_state(e, JBWM_EWMH_WM_STATE_STICKY, c);
}
