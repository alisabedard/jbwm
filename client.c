// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "JBWMEnv.h"
#include "log.h"
#include "screen.h"
#include "titlebar.h"
#include "util.h"
#include <X11/Xatom.h>
// Free result with XFree if not NULL
char * jbwm_get_title(const jbwm_window_t w)
{
	XTextProperty tp;
	if (!XGetWMName(jbwm.d, w, &tp))
		  return NULL;
	return (char *)tp.value;
}
void jbwm_set_client_vdesk(struct JBWMClient * restrict c, const uint8_t d)
{
	LOG("jbwm_set_client_vdesk");
	const uint8_t p = c->vdesk;
	c->vdesk = d;
	// use jbwm_set_vdesk to validate d:
	c->vdesk = jbwm_set_vdesk(c->screen, d);
	jbwm_set_vdesk(c->screen, p);
}
static struct JBWMClient * search(struct JBWMClient * c,
	const jbwm_window_t w)
{
#ifdef USE_TBAR
	return (!c || c->parent == w || c->window == w || c->tb.win == w)
#else//!USE_TBAR
	return (!c || c->parent == w || c->window == w)
#endif//USE_TBAR
		? c : search(c->next, w);
}
// Return the client that has specified window as either window or parent
struct JBWMClient * jbwm_get_client(const jbwm_window_t w)
{
	return search(jbwm.head, w);
}
static void unselect_current(void)
{
	if(!jbwm.current) return;
	XSetWindowBorder(jbwm.d, jbwm.current->parent,
		jbwm.current->screen->pixels.bg);
#ifdef JBWM_USE_EWMH
	ewmh_remove_state(jbwm.current->window, ewmh[WM_STATE_FOCUSED]);
#endif//JBWM_USE_EWMH
}
static void set_border(struct JBWMClient * restrict c)
{
	XSetWindowBorder(jbwm.d, c->parent, c->opt.sticky
		? c->screen->pixels.fc : c->screen->pixels.fg);
}
void jbwm_select_client(struct JBWMClient * c)
{
	if(!c)
		return;
	unselect_current();
	XInstallColormap(jbwm.d, c->cmap);
	XSetInputFocus(jbwm.d, c->window, RevertToPointerRoot, CurrentTime);
	set_border(c);
	jbwm.current = c;
#ifdef JBWM_USE_EWMH
	jbwm_set_property(c->screen->root, ewmh[ACTIVE_WINDOW],
		XA_WINDOW, &(c->parent), 1);
	ewmh_add_state(c->window, ewmh[WM_STATE_FOCUSED]);
#endif//JBWM_USE_EWMH
}
void jbwm_toggle_sticky(struct JBWMClient * c)
{
	LOG("stick");
	c->opt.sticky ^= true; // toggle
	jbwm_select_client(c);
	jbwm_update_titlebar(c);
#ifdef JBWM_USE_EWMH
	(c->opt.sticky ? ewmh_add_state : ewmh_remove_state)(c->window,
		ewmh[WM_STATE_STICKY]);
#endif//JBWM_USE_EWMH
}
// Returns 0 on failure.
static Status xmsg(const jbwm_window_t w, const Atom a, const long x)
{
	LOG("xmsg");
	return XSendEvent(jbwm.d, w, false, NoEventMask, &(XEvent){
		.xclient.type = ClientMessage, .xclient.window = w,
		.xclient.message_type = a, .xclient.format = 32,
		.xclient.data.l[0] = x, .xclient.data.l[1] = CurrentTime
	});
}
static jbwm_atom_t get_atom(jbwm_atom_t * a, const char * name)
{
	return *a ? *a : (*a = XInternAtom(jbwm.d, name, false));
}
static jbwm_atom_t get_wm_protocols(void)
{
	static jbwm_atom_t a;
	return get_atom(&a, "WM_PROTOCOLS");
}
static jbwm_atom_t get_wm_delete_window(void)
{
	static jbwm_atom_t a;
	return get_atom(&a, "WM_DELETE_WINDOW");
}
jbwm_atom_t jbwm_get_wm_state(void)
{
	static jbwm_atom_t a;
	return get_atom(&a, "WM_STATE");
}
void jbwm_set_wm_state(struct JBWMClient * restrict c, const int8_t state)
{
	jbwm_set_property(c->window, jbwm_get_wm_state(),
		XA_CARDINAL, &(uint32_t){state}, 1);
}
static bool has_delete_proto(const jbwm_window_t w)
{
	bool found=false;
	Atom *p;
	int i;
	if(XGetWMProtocols(jbwm.d, w, &p, &i)) {
		while(i-- && !found)
			found = p[i] == get_wm_delete_window();
		XFree(p);
	}
	return found;
}
void jbwm_send_wm_delete(struct JBWMClient * restrict c)
{
	if (c->opt.remove) { // this allows a second click to force a kill
		XKillClient(jbwm.d, c->window);
		return;
	}
	c->opt.remove = true;
	has_delete_proto(c->window)?xmsg(c->window, get_wm_protocols(),
		get_wm_delete_window()): XKillClient(jbwm.d, c->window);
}
