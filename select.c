// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "select.h"
#include <X11/Xatom.h>
#include "client.h"
#include "display.h"
#include "ewmh_state.h"
#include "ewmh.h"
#include "log.h"
#include "screen.h"
#include "util.h"
#define EWMH_ATOM(a) jbwm_ewmh_get_atom(JBWM_EWMH_##a)
#define WM_STATE(a) EWMH_ATOM(WM_STATE_##a)
static void set_state_not_focused(struct JBWMClient * c)
{
	Display * d = jbwm_get_display();
	XSetWindowBorder(d, c->parent,
		jbwm_get_screen(c)->pixels.bg);
	jbwm_ewmh_remove_state(d, c->window, WM_STATE(FOCUSED));
}
static bool check_current(struct JBWMClient * c)
{
	struct JBWMClientManager * m = jbwm_get_client_manager();
	if (m->current) {
		if (m->current == c)
			return false;
		return true;
	}
	return false;
}
static void unselect_current(struct JBWMClient * c)
{
	if (check_current(c))
		set_state_not_focused(jbwm_get_current_client());
}
static void set_border(struct JBWMClient * c)
{
	struct JBWMScreen * s = jbwm_get_screen(c);
	XSetWindowBorder(jbwm_get_display(), c->parent, c->opt.sticky
		? s->pixels.fc : s->pixels.fg);
}
static void set_focused(struct JBWMClient * c)
{
	Display * d = jbwm_get_display();
	XInstallColormap(d, c->cmap);
	XSetInputFocus(d, c->window, RevertToPointerRoot, CurrentTime);
	jbwm_ewmh_add_state(d, c->window, WM_STATE(FOCUSED));
}
static void set_active_window(struct JBWMClient * c)
{
	/* Store the window id as a static variable here in case
	 * client c is freed before the X server handles the event.
	 * If the property is read after the client is freed, it will
	 * cause a segmentation fault.  */
	static Window w;
	w = c->window;
	struct JBWMClientManager * m = jbwm_get_client_manager();
	jbwm_set_property(jbwm_get_display(), jbwm_get_root(c),
		EWMH_ATOM(ACTIVE_WINDOW), XA_WINDOW, &w, 1);
	m->current = c;
}
void jbwm_select_client(struct JBWMClient * c)
{
	if (c) {
		unselect_current(c);
		set_border(c);
		set_focused(c);
		set_active_window(c);
	}
}

