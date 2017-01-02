// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "client.h"
#include "display.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "JBWMClient.h"
#include "log.h"
#include "screen.h"
#include "select.h"
#include "title_bar.h"
static struct JBWMClient * current, * head;
struct JBWMClient * jbwm_get_current_client(void)
{
	return current;
}
void jbwm_set_current_client(struct JBWMClient * restrict c)
{
	current = c;
}
struct JBWMClient * jbwm_get_head_client(void)
{
	return head;
}
void jbwm_prepend_client(struct JBWMClient * restrict c)
{
	c->next = head;
	head = c;
}
/* Note:  As *c and *i may alias each other, use of 'restrict'
   in relink_r would be invalid. */
static void relink_r(const struct JBWMClient * c, struct JBWMClient * i)
{
	if (i && i->next)
		relink_r(c, i->next != c ? i->next : (i->next = c->next));
}
// Relink c's linked list to exclude c
void jbwm_relink_client_list(struct JBWMClient * restrict c)
{
	JBWM_LOG("relink_window_list");
	if (current == c) // Remove selection target
		current = NULL;
	if (head == c) {
		head = c->next;
		return; // removed first client
	}
	relink_r(c, head);
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
static inline bool matches(struct JBWMClient * i,
	const Window w)
{
#ifdef JBWM_USE_TITLE_BAR
	return i->parent == w || i->window == w || i->tb.win == w;
#else//!JBWM_USE_TITLE_BAR
	return i->parent == w || i->window == w;
#endif//JBWM_USE_TITLE_BAR
}
__attribute__((pure))
static struct JBWMClient * search(struct JBWMClient * restrict c,
	const Window w)
{
	return (!c || matches(c, w)) ? c : search(c->next, w);
}
// Return the client that has specified window as either window or parent
__attribute__((pure))
struct JBWMClient * jbwm_get_client(const Window w)
{
	return search(head, w);
}
void jbwm_toggle_sticky(struct JBWMClient * restrict c)
{
	JBWM_LOG("stick");
	c->opt.sticky ^= true; // toggle
	jbwm_select_client(c);
	jbwm_update_title_bar(c);
#ifdef JBWM_USE_EWMH
	(c->opt.sticky ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
		(jbwm_get_display(), c->window,jbwm_ewmh_get_atom(
		  JBWM_EWMH_WM_STATE_STICKY));
#endif//JBWM_USE_EWMH
}
