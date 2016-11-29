// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "events.h"
#include <stdlib.h>
#include <X11/Xatom.h>
#include "button_event.h"
#include "client.h"
#include "ewmh_state.h"
#include "ewmh.h"
#include "jbwm.h"
#include "keys.h"
#include "log.h"
#include "macros.h"
#include "new.h"
#include "screen.h"
#include "title_bar.h"
#include "util.h"
#include "wm_state.h"
#define DEBUG_EVENTS
#ifndef DEBUG_EVENTS
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!DEBUG_EVENTS
//#define DEBUG_ATOMS
#ifndef DEBUG_ATOMS
#undef jbwm_print_atom
#define jbwm_print_atom(...)
#endif//!DEBUG_ATOMS
static bool events_need_cleanup;
static jbwm_window_t last_window;
__attribute__((pure))
static struct JBWMScreen * get_screen(const int8_t i,
	const jbwm_window_t root)
{
	struct JBWMScreen * s = jbwm_get_screens();
	return s[i].root == root ? s + i
		: get_screen(i - 1, root);
}
#ifdef JBWM_USE_EWMH
static void delete_ewmh_properties(Display * restrict d,
	const jbwm_window_t w)
{
	// Per ICCCM + wm-spec
	XDeleteProperty(d, w, jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE));
	XDeleteProperty(d, w, jbwm_ewmh_get_atom(JBWM_EWMH_WM_DESKTOP));
}
#else//!JBWM_USE_EWMH
#define delete_ewmh_properties(d, w)
#endif//JBWM_USE_EWMH
void jbwm_free_client(struct JBWMClient * restrict c)
{
	Display * d = c->d;
	{ // w scope
		const jbwm_window_t w = c->window;
		delete_ewmh_properties(d, w);
		XReparentWindow(d, w, jbwm_get_screens()[c->screen].root,
			c->size.x, c->size.y);
		XRemoveFromSaveSet(d, w);
	}
	if(c->parent)
		XDestroyWindow(d, c->parent);
	jbwm_relink_client_list(c);
	free(c);
	// Allow future clients with the same window ID:
	last_window = 0;
}
static void cleanup(void)
{
	JBWM_LOG("cleanup");
	events_need_cleanup = false;
	struct JBWMClient * c = jbwm_get_head_client();
	struct JBWMClient * i;
	do {
		i = c->next;
		if (!c->opt.remove)
			continue;
		jbwm_free_client(c);
	} while(i && (c = i));
}
static void handle_property_change(XPropertyEvent * restrict e,
	struct JBWMClient * restrict c)
{
	jbwm_print_atom(e->display, e->atom, __FILE__, __LINE__);
	if(e->state != PropertyNewValue)
		return;
	if (e->atom == XA_WM_NAME)
		jbwm_update_title_bar(c);
	else {
		if (e->atom == jbwm_get_wm_state(e->display))
			jbwm_move_resize(c);
		jbwm_print_atom(e->display, e->atom,
			__FILE__, __LINE__);
	}
}
static void handle_configure_request(XConfigureRequestEvent * restrict e)
{
	JBWM_LOG("handle_configure_request():"
		"x: %d, y: %d, w: %d, h: %d",
		e->x, e->y, e->width, e->height);
	XConfigureWindow(e->display, e->window, e->value_mask,
		&(XWindowChanges){ .x = e->x, .y = e->y,
		.width = e->width, .height = e->height,
		.sibling = e->above, .stack_mode = e->detail});
}
static void handle_map_request(XMapRequestEvent * restrict e)
{
	/* This check fixes a race condition in old libreoffice and certain
	   Motif dialogs where an attempt is made to request mapping twice: */
	const jbwm_window_t w = e->window;
	if (w == last_window)
		return;
	last_window = w;
	JBWM_LOG("MapRequest, send_event:%d", e->send_event);
	Display * restrict d = e->display;
	jbwm_new_client(d, get_screen(ScreenCount(d), e->parent), w);
}
static inline void mark_removal(struct JBWMClient * restrict c)
{
	JBWM_LOG("mark_removal(): ignore_unmap is %d", c->ignore_unmap);
	c->opt.remove = events_need_cleanup = (c->ignore_unmap--<1);
}
void jbwm_event_loop(Display * restrict d)
{
	XEvent ev;
event_loop:
	XNextEvent(d, &ev);
	struct JBWMClient * restrict c = jbwm_get_client(ev.xany.window);
	switch (ev.type) {
	case ButtonRelease:
	case KeyRelease:
	case MapNotify:
	case MappingNotify:
	case MotionNotify:
		// ignore
		goto event_loop;
	case ConfigureNotify:
		JBWM_LOG("ConfigureNotify");
		/* Failure to do this causes Java Swing applications to first
		 appear offset within the parent window until they receive
		 a click.  This is a fix:  */
		if (c)
			jbwm_move_resize(c);
		goto event_loop;
	case ReparentNotify:
		JBWM_LOG("ReparentNotify");
		/* Reset last_window to allow other clients
		   with the same window id to be started.  */
		last_window = 0;
		goto event_loop;
	case KeyPress:
		jbwm_handle_key_event(&ev.xkey);
		break;
	case ButtonPress:
		if(c)
			jbwm_handle_button_event(&ev.xbutton, c);
		break;
	case EnterNotify:
		if(c && (ev.xcrossing.window == c->parent))
			jbwm_select_client(c);
		goto event_loop;
#ifdef JBWM_USE_TITLE_BAR
	case Expose:
		if (c && !ev.xexpose.count)
			jbwm_update_title_bar(c);
		goto event_loop;
#endif//JBWM_USE_TITLE_BAR
#ifdef JBWM_USE_EWMH
	case CreateNotify:
		JBWM_LOG("CreateNotify");
		JBWM_LOG("override_redirect: %d",
			(int)ev.xcreatewindow.override_redirect);
		JBWM_LOG("send_event: %d", (int)ev.xcreatewindow.send_event);
		if (ev.xcreatewindow.override_redirect) // internal
			jbwm_ewmh_update_client_list(ev.xany.display);
		break;
	case DestroyNotify:
		JBWM_LOG("DestroyNotify");
		if (!c) // only bother if event was not on a client
			jbwm_ewmh_update_client_list(ev.xany.display);
		break;
#endif//JBWM_USE_EWMH
	case UnmapNotify:
		if (c)
			mark_removal(c);
		break;
	case MapRequest:
		if (!c)
			handle_map_request(&ev.xmaprequest);
		break;
	case ConfigureRequest:
		handle_configure_request(&ev.xconfigurerequest);
		goto event_loop;
	case PropertyNotify:
		if (c)
			handle_property_change(&ev.xproperty, c);
		goto event_loop;
	case ColormapNotify:
		if (c && ev.xcolormap.new) {
			JBWM_LOG("ColormapNotify");
			c->cmap = ev.xcolormap.colormap;
			XInstallColormap(ev.xcolormap.display, c->cmap);
		}
		goto event_loop;
#ifdef JBWM_USE_EWMH
	case ClientMessage:
		jbwm_ewmh_handle_client_message(&ev.xclient, c);
		break;
#endif//JBWM_USE_EWMH
#ifdef DEBUG_EVENTS
	default:
		JBWM_LOG("Unhandled event (%d)", ev.type);
#endif//DEBUG_EVENTS
	}
	if (events_need_cleanup)
		cleanup();
	goto event_loop;
}

