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
/* Note:  As *c and *i may alias each other, use of 'restrict'
   in relink_r would be invalid. */
__attribute__((nonnull(1)))
static void relink_r(const struct JBWMClient * c, struct JBWMClient * i)
{
	if (i && i->next) {
		if (i->next == c) // skip over c:
			i->next = c->next;
		else
			relink_r(c, i->next);
	}
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
	relink_r(c, jbwm_client_data.head);
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
static void set_state_not_focused(struct JBWMClient * restrict c)
{
	XSetWindowBorder(c->d, c->parent, jbwm_get_screen(c)->pixels.bg);
	jbwm_ewmh_remove_state(c->d, c->window, jbwm_ewmh_get_atom(
		JBWM_EWMH_WM_STATE_FOCUSED));
}
static bool check_current(void)
{
	if (jbwm_client_data.current) {
		if (!jbwm_client_data.current->d) {
			jbwm_client_data.current = NULL;
			return false;
		}
		return true;
	}
	return false;
}
static void unselect_current(void)
{
	if (check_current())
		set_state_not_focused(jbwm_client_data.current);
}
static void set_border(struct JBWMClient * restrict c)
{
	struct JBWMScreen * restrict s = jbwm_get_screen(c);
	XSetWindowBorder(c->d, c->parent, c->opt.sticky ? s->pixels.fc
		: s->pixels.fg);
}
static void set_focused(struct JBWMClient * c)
{
	Display * restrict d = c->d;
	XInstallColormap(c->d, c->cmap);
	XSetInputFocus(d, c->window, RevertToPointerRoot, CurrentTime);
	jbwm_ewmh_add_state(d, c->window, jbwm_ewmh_get_atom(
		JBWM_EWMH_WM_STATE_FOCUSED));
}
static void set_active_window(struct JBWMClient * c)
{
	unselect_current(); // depends on jbwm_client_data.current
	/* Store the window id as a static variable here in case
	 * client c is freed.  If the property is read after the
	 * client is freed, it will cause a segmentation fault.  */
	static jbwm_window_t w;
	w = c->window;
	jbwm_set_property(c->d, jbwm_get_root(c),
		jbwm_ewmh_get_atom(JBWM_EWMH_ACTIVE_WINDOW),
		XA_WINDOW, &w, 1);
	jbwm_client_data.current = c;
}
void jbwm_select_client(struct JBWMClient * c)
{
	if (c) {
		set_border(c);
		set_focused(c);
		set_active_window(c);
	}
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
