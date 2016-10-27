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
#include "title_bar.h"
#include "util.h"
#include <X11/Xatom.h>
static struct {
	struct JBWMClient * current, * head;
} jbwm_client_data;
struct JBWMClient * jbwm_get_current_client(void)
{
	return jbwm_client_data.current;
}
struct JBWMClient * jbwm_get_head_client(void)
{
	return jbwm_client_data.head;
}
void jbwm_set_head_client(struct JBWMClient * restrict c)
{
	if (c)
		jbwm_client_data.head = c;
}
// Relink c's linked list to exclude c
void jbwm_relink_client_list(struct JBWMClient * c)
{
	JBWM_LOG("relink_window_list");
	if (jbwm_client_data.current == c) // Remove selection target
		jbwm_client_data.current = NULL;
	if (jbwm_client_data.head == c) {
		jbwm_client_data.head = c->next;
		return;
	}
	for (struct JBWMClient * p = jbwm_client_data.head;
		p && p->next; p = p->next) {
		if (p->next == c) { // Close the link
			p->next = c->next;
			return;
		}
	}
}
// Free result with XFree if not NULL
char * jbwm_get_title(const jbwm_window_t w)
{
	XTextProperty tp;
	if (!XGetWMName(jbwm_get_display(), w, &tp))
		  return NULL;
	return (char *)tp.value;
}
void jbwm_set_client_vdesk(struct JBWMClient * restrict c, const uint8_t d)
{
	JBWM_LOG("jbwm_set_client_vdesk");
	const uint8_t p = c->vdesk;
	c->vdesk = d;
	// use jbwm_set_vdesk to validate d:
	c->vdesk = jbwm_set_vdesk(c->screen, d);
	jbwm_set_vdesk(c->screen, p);
}
static struct JBWMClient * search(struct JBWMClient * c,
	const jbwm_window_t w)
{
#ifdef JBWM_USE_TITLE_BAR
	return (!c || c->parent == w || c->window == w || c->tb.win == w)
#else//!JBWM_USE_TITLE_BAR
	return (!c || c->parent == w || c->window == w)
#endif//JBWM_USE_TITLE_BAR
		? c : search(c->next, w);
}
// Return the client that has specified window as either window or parent
struct JBWMClient * jbwm_get_client(const jbwm_window_t w)
{
	return search(jbwm_client_data.head, w);
}
static void unselect_current(void)
{
	if(!jbwm_client_data.current) return;
	XSetWindowBorder(jbwm_get_display(), jbwm_client_data.current->parent,
		jbwm_client_data.current->screen->pixels.bg);
#ifdef JBWM_USE_EWMH
	jbwm_ewmh_remove_state(jbwm_client_data.current->window,
		ewmh[WM_STATE_FOCUSED]);
#endif//JBWM_USE_EWMH
}
static void set_border(struct JBWMClient * restrict c)
{
	XSetWindowBorder(jbwm_get_display(), c->parent, c->opt.sticky
		? c->screen->pixels.fc : c->screen->pixels.fg);
}
void jbwm_select_client(struct JBWMClient * c)
{
	if(!c)
		return;
	unselect_current();
	XInstallColormap(jbwm_get_display(), c->cmap);
	XSetInputFocus(jbwm_get_display(), c->window, RevertToPointerRoot, CurrentTime);
	set_border(c);
	jbwm_client_data.current = c;
#ifdef JBWM_USE_EWMH
	jbwm_set_property(c->screen->root, ewmh[ACTIVE_WINDOW],
		XA_WINDOW, &(c->parent), 1);
	jbwm_ewmh_add_state(c->window, ewmh[WM_STATE_FOCUSED]);
#endif//JBWM_USE_EWMH
}
void jbwm_toggle_sticky(struct JBWMClient * c)
{
	JBWM_LOG("stick");
	c->opt.sticky ^= true; // toggle
	jbwm_select_client(c);
	jbwm_update_title_bar(c);
#ifdef JBWM_USE_EWMH
	(c->opt.sticky ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)(c->window,
		ewmh[WM_STATE_STICKY]);
#endif//JBWM_USE_EWMH
}
// Returns 0 on failure.
static Status xmsg(const jbwm_window_t w, const Atom a, const long x)
{
	JBWM_LOG("xmsg");
	return XSendEvent(jbwm_get_display(), w, false, NoEventMask, &(XEvent){
		.xclient.type = ClientMessage, .xclient.window = w,
		.xclient.message_type = a, .xclient.format = 32,
		.xclient.data.l[0] = x, .xclient.data.l[1] = CurrentTime
	});
}
static jbwm_atom_t get_atom(jbwm_atom_t * a, const char * name)
{
	return *a ? *a : (*a = XInternAtom(jbwm_get_display(), name, false));
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
	if(XGetWMProtocols(jbwm_get_display(), w, &p, &i)) {
		while(i-- && !found)
			found = p[i] == get_wm_delete_window();
		XFree(p);
	}
	return found;
}
void jbwm_send_wm_delete(struct JBWMClient * restrict c)
{
	if (c->opt.remove) { // this allows a second click to force a kill
		XKillClient(jbwm_get_display(), c->window);
		return;
	}
	c->opt.remove = true;
	has_delete_proto(c->window)?xmsg(c->window, get_wm_protocols(),
		get_wm_delete_window()): XKillClient(jbwm_get_display(), c->window);
}
