// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "select.h"
#include <X11/Xatom.h>
#include "client.h"
#include "ewmh_state.h"
#include "ewmh.h"
#include "log.h"
#include "screen.h"
#include "util.h"
static void set_state_not_focused(struct JBWMClient * restrict c)
{
	XSetWindowBorder(c->d, c->parent, jbwm_get_screen(c)->pixels.bg);
	jbwm_ewmh_remove_state(c->d, c->window, jbwm_ewmh_get_atom(
		JBWM_EWMH_WM_STATE_FOCUSED));
}
static bool check_current(void)
{
	struct JBWMClientManager * m = jbwm_get_client_manager();
	if (m->current) {
		if (!m->current->d) {
			m->current = NULL;
			return false;
		}
		return true;
	}
	return false;
}
static void unselect_current(void)
{
	if (check_current())
		set_state_not_focused(jbwm_get_current_client());
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
	unselect_current(); // depends on m->current
	/* Store the window id as a static variable here in case
	 * client c is freed before the X server handles the event.
	 * If the property is read after the client is freed, it will
	 * cause a segmentation fault.  */
	static Window w;
	w = c->window;
	/* Store a static version of the display pointer here to
	 * verify the client's version.  */
	static Display * d;
	if (!d)
		d = c->d;
	struct JBWMClientManager * m = jbwm_get_client_manager();
	if (d != c->d) {
		/* If this is reached, the display pointer within c
		 * has become corrupt.  This is due to an event loop
		 * race condition on window unmap.  In this case,
		 * cease processing setting of the active window to c.
		 * Then, set the current client to the head client.
		 * Finally, return.  */
		JBWM_LOG("Unmap race caught, safely handled.");
		m->current = m->head;
		return;
	}
	jbwm_set_property(d, jbwm_get_root(c),
		jbwm_ewmh_get_atom(JBWM_EWMH_ACTIVE_WINDOW),
		XA_WINDOW, &w, 1);
	m->current = c;
}
void jbwm_select_client(struct JBWMClient * c)
{
	if (c) {
		set_border(c);
		set_focused(c);
		set_active_window(c);
	}
}
