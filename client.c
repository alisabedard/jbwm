// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "jbwm.h"
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
		return; // removed first client
	}
	for (struct JBWMClient * p = jbwm_client_data.head;
		p && p->next; p = p->next) {
		if (p->next == c) { // Close the link
			p->next = c->next;
			return;
		}
	}
}
void jbwm_set_client_vdesk(struct JBWMClient * restrict c, const uint8_t d)
{
	JBWM_LOG("jbwm_set_client_vdesk");
	const uint8_t p = c->vdesk; // save previous
	c->vdesk = d;
	// use jbwm_set_vdesk to validate d:
	struct JBWMScreen * s = &jbwm_get_screens()[c->screen];
	c->vdesk = jbwm_set_vdesk(s, d);
	jbwm_set_vdesk(s, p);
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
static void unselect_current(Display * restrict d)
{
	if(!jbwm_client_data.current)
		return;
	XSetWindowBorder(d, jbwm_client_data.current->parent,
		jbwm_get_screens()[jbwm_client_data.current->screen]
		.pixels.bg);
#ifdef JBWM_USE_EWMH
	jbwm_ewmh_remove_state(d, jbwm_client_data.current->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_FOCUSED));
#endif//JBWM_USE_EWMH
}
static void set_border(Display * restrict d, struct JBWMClient * restrict c)
{
	struct JBWMScreen * restrict s = &jbwm_get_screens()[c->screen];
	XSetWindowBorder(d, c->parent, c->opt.sticky
		? s->pixels.fc : s->pixels.fg);
}
void jbwm_select_client(struct JBWMClient * c)
{
	if(!c)
		return;
	Display * d = c->d;
	unselect_current(d);
	XInstallColormap(d, c->cmap);
	XSetInputFocus(d, c->window,
		RevertToPointerRoot, CurrentTime);
	set_border(d, c);
	jbwm_client_data.current = c;
#ifdef JBWM_USE_EWMH
	jbwm_set_property(d, jbwm_get_root(c),
		jbwm_ewmh_get_atom(JBWM_EWMH_ACTIVE_WINDOW),
		XA_WINDOW, &(c->parent), 1);
	jbwm_ewmh_add_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_FOCUSED));
#endif//JBWM_USE_EWMH
}
void jbwm_toggle_sticky(struct JBWMClient * c)
{
	JBWM_LOG("stick");
	c->opt.sticky ^= true; // toggle
	jbwm_select_client(c);
	jbwm_update_title_bar(c);
#ifdef JBWM_USE_EWMH
	(c->opt.sticky ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
		(c->d, c->window,jbwm_ewmh_get_atom(
		  JBWM_EWMH_WM_STATE_STICKY));
#endif//JBWM_USE_EWMH
}
