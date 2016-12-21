// Copyright 2016, Jeffrey E. Bedard
#include "wm_state.h"
#include <X11/Xatom.h>
#include "display.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "log.h"
#include "util.h"
// Returns 0 on failure.
static Status xmsg(Display * restrict d, const Window w,
	const Atom a, const long x)
{
	JBWM_LOG("xmsg");
	XClientMessageEvent e = { .type = ClientMessage, .window = w,
		.message_type = a, .format = 32 };
	// Split initialization to avoid old gcc warnings.
	{ // * l scope
		long * restrict l = e.data.l;
		l[0] = x;
		l[1] = CurrentTime;
	}
	return XSendEvent(d, w, false, NoEventMask, (XEvent *)&e);
}
static Atom get_atom(Display * restrict d,
	Atom * restrict a, const char * restrict name)
{
	return *a ? *a : (*a = XInternAtom(d, name, false));
}
static Atom get_wm_protocols(Display * d)
{
	static Atom a;
	return get_atom(d, &a, "WM_PROTOCOLS");
}
static Atom get_wm_delete_window(Display * d)
{
	static Atom a;
	return get_atom(d, &a, "WM_DELETE_WINDOW");
}
Atom jbwm_get_wm_state(Display * d)
{
	static Atom a;
	return get_atom(d, &a, "WM_STATE");
}
void jbwm_set_wm_state(struct JBWMClient * restrict c, const int8_t state)
{
	Display * d = jbwm_get_display();
	const Window w = c->window;
	jbwm_set_property(d, w, jbwm_get_wm_state(d), XA_CARDINAL,
		&(uint32_t){state}, 1);
#ifdef JBWM_USE_EWMH
	static Atom a;
	if (!a)
		a = jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_HIDDEN);
	if (state == IconicState)
		jbwm_ewmh_add_state(d, w, a);
	else
		jbwm_ewmh_remove_state(d, w, a);
#endif//JBWM_USE_EWMH
}
static bool has_delete_proto(Display * d, const Window w)
{
	bool found=false;
	Atom *p;
	int i;
	if (XGetWMProtocols(d, w, &p, &i)) {
		while(i-- && !found)
			found = p[i] == get_wm_delete_window(d);
		XFree(p);
	}
	return found;
}
void jbwm_send_wm_delete(struct JBWMClient * restrict c)
{
	Display * d = jbwm_get_display();
	const Window w = c->window;
	if (c->opt.remove) { // this allows a second click to force a kill
		XKillClient(d, w);
		return;
	}
	c->opt.remove = true;
	has_delete_proto(d, w) ? xmsg(d, w, get_wm_protocols(d),
		get_wm_delete_window(d)) : XKillClient(d, w);
	XSync(d, false); // Process everything now.
}
