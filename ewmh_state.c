// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#undef DEBUG
#include "ewmh_state.h"
#include <X11/Xatom.h>
#include "JBWMAtomIndex.h"
#include "client.h"
#include "drag.h"
#include "ewmh.h"
#include "jbwm.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "select.h"
#include "util.h"
#include "wm_state.h"
static void nullify(const int n, const Atom state, Atom * restrict a)
{
	if (n) {
		if (a[n] == state)
			a[n] = 0;
		nullify(n - 1, state, a);
	}
}
// Remove specified atom from WM_STATE
void jbwm_ewmh_remove_state(Display * d,
	const Window w, const Atom state)
{
	uint16_t n;
	const Atom ws = jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE);
	Atom *a = jbwm_get_property(d, w, ws, &n);
	if (!a)
		return;
	nullify(n, state, a);
	jbwm_set_property(d, w, ws, XA_ATOM, a, n);
	XFree(a);
}
bool jbwm_ewmh_get_state(Display * d,
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
void jbwm_ewmh_add_state(Display * d, const Window w,
	Atom state)
{
	XChangeProperty(d, w, jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE),
		XA_ATOM, 32, PropModePrepend,
		(unsigned char *)&state, 1);
}

