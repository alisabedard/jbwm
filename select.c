// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "select.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include "client.h"
#include "display.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "screen.h"
#include "util.h"
#define EWMH_ATOM(a) jbwm_ewmh_get_atom(JBWM_EWMH_##a)
#define WM_STATE(a) EWMH_ATOM(WM_STATE_##a)
static inline jbwm_pixel_t get_bg(struct JBWMClient * restrict c)
{
	return jbwm_get_screen(c)->pixels.bg;
}
static void set_state_not_focused(Display * d,
	struct JBWMClient * restrict c)
{
	XSetWindowBorder(d, c->parent, get_bg(c));
	jbwm_ewmh_remove_state(d, c->window, WM_STATE(FOCUSED));
}
static void unselect_current(Display * d, struct JBWMClient * restrict c)
{
	struct JBWMClient * current = jbwm_get_current_client();
	if (current && current != c)
		set_state_not_focused(d, current);
}
static void set_border(Display * d, struct JBWMClient * restrict c)
{
	struct JBWMPixels * restrict p = &jbwm_get_screen(c)->pixels;
	XSetWindowBorder(d, c->parent, c->opt.sticky ? p->fc : p->fg);
}
static void set_focused(Display * d, struct JBWMClient * restrict c)
{
	XInstallColormap(d, c->cmap);
	const Window w = c->window;
	XSetInputFocus(d, w, RevertToPointerRoot, CurrentTime);
	jbwm_ewmh_add_state(d, w, WM_STATE(FOCUSED));
}
static void set_active_window(Display * d, struct JBWMClient * restrict c)
{
	/* Store the window id as a static variable here in case
	 * client c is freed before the X server handles the event.
	 * If the property is read after the client is freed, it will
	 * cause a segmentation fault.  */
#ifdef JBWM_USE_EWMH
	static Window w;
	w = c->window;
	jbwm_set_property(d, jbwm_get_root(c),
		EWMH_ATOM(ACTIVE_WINDOW), XA_WINDOW, &w, 1);
#endif//JBWM_USE_EWMH
	jbwm_set_current_client(c);
}
void jbwm_select_client(struct JBWMClient * restrict c)
{
	if (c) {
		Display * d = jbwm_get_display();
		unselect_current(d, c);
		set_border(d, c);
		set_focused(d, c);
		set_active_window(d, c);
	}
}

