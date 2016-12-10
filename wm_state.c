// Copyright 2016, Jeffrey E. Bedard
#include "wm_state.h"
#include <X11/Xatom.h>
#include "log.h"
#include "util.h"
// Returns 0 on failure.
static Status xmsg(Display * d, const jbwm_window_t w,
	const Atom a, const long x)
{
	JBWM_LOG("xmsg");
	return XSendEvent(d, w, false, NoEventMask, &(XEvent){
		.xclient.type = ClientMessage, .xclient.window = w,
		.xclient.message_type = a, .xclient.format = 32,
		.xclient.data.l[0] = x, .xclient.data.l[1] = CurrentTime
	});
}
static jbwm_atom_t get_atom(Display * d,
	jbwm_atom_t * a, const char * name)
{
	return *a ? *a : (*a = XInternAtom(d, name, false));
}
static jbwm_atom_t get_wm_protocols(Display * d)
{
	static jbwm_atom_t a;
	return get_atom(d, &a, "WM_PROTOCOLS");
}
static jbwm_atom_t get_wm_delete_window(Display * d)
{
	static jbwm_atom_t a;
	return get_atom(d, &a, "WM_DELETE_WINDOW");
}
jbwm_atom_t jbwm_get_wm_state(Display * d)
{
	static jbwm_atom_t a;
	return get_atom(d, &a, "WM_STATE");
}
void jbwm_set_wm_state(struct JBWMClient * c, const int8_t state)
{
	Display * d = c->d;
	jbwm_set_property(d, c->window, jbwm_get_wm_state(d),
		XA_CARDINAL, &(uint32_t){state}, 1);
}
static bool has_delete_proto(Display * d, const jbwm_window_t w)
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
void jbwm_send_wm_delete(struct JBWMClient * c)
{
	Display * d = c->d;
	const jbwm_window_t w = c->window;
	if (c->opt.remove) { // this allows a second click to force a kill
		XKillClient(d, w);
		return;
	}
	c->opt.remove = true;
	has_delete_proto(d, w) ? xmsg(d, w, get_wm_protocols(d),
		get_wm_delete_window(d)) : XKillClient(d, w);
}
