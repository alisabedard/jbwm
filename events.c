// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#define LOG_LEVEL 0
#if LOG_LEVEL == 0
#undef DEBUG
#endif//LOG_LEVEL
#include "events.h"
#include <X11/Xatom.h>
#include "button_event.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_client.h"
#include "keys.h"
#include "log.h"
#include "move_resize.h"
#include "new.h"
#include "screen.h"
#include "select.h"
#include "title_bar.h"
#include "util.h"
#include "wm_state.h"
#if LOG_LEVEL > 7
#define ELOG(...) JBWM_LOG(__VA_ARGS__)
#else//LOG_LEVEL<=6
#define ELOG(...)
#endif//LOG_LEVEL>7
static bool events_need_cleanup;
static Window last_window;
__attribute__((pure))
static struct JBWMScreen * get_screen(const int8_t i,
	const Window root)
{
	struct JBWMScreen * s = jbwm_get_screens();
	return s[i].root == root ? s + i : get_screen(i - 1, root);
}
static void cleanup(Display * d, struct JBWMClient * i)
{
	JBWM_LOG("cleanup");
	if (!i)
		return;
	struct JBWMClient * next = i->next; // save
	if (i->opt.remove)
		jbwm_client_free(d, i);
	cleanup(d, next);
}
static void handle_property_change(XPropertyEvent * e,
	struct JBWMClient * restrict c)
{
	if(e->state != PropertyNewValue)
		return;
	if (e->atom == XA_WM_NAME)
		jbwm_update_title_bar(e->display, c);
	else {
		jbwm_print_atom(e->display, e->atom, __FILE__, __LINE__);
		if (e->atom == jbwm_get_wm_state(e->display))
			jbwm_move_resize(e->display, c);
	}
}
static void handle_configure_request(XConfigureRequestEvent * e)
{
#if LOG_LEVEL > 4
	JBWM_LOG("handle_configure_request():"
		"x: %d, y: %d, w: %d, h: %d, b: %d",
		e->x, e->y, e->width, e->height, e->border_width);
#endif//LOG_LEVEL
	XConfigureWindow(e->display, e->window, e->value_mask,
		&(XWindowChanges){ .x = e->x, .y = e->y,
		.width = e->width, .height = e->height,
		.border_width = e->border_width,
		.sibling = e->above, .stack_mode = e->detail});
}
static void handle_map_request(XMapRequestEvent * e)
{
	/* This check fixes a race condition in old libreoffice and certain
	   Motif dialogs where an attempt is made to request mapping twice: */
	const Window w = e->window;
	if (w == last_window)
		return;
	last_window = w;
	JBWM_LOG("MapRequest, send_event:%d", e->send_event);
	jbwm_new_client(e->display, get_screen(ScreenCount(e->display),
		e->parent), w);
}
static inline void mark_removal(struct JBWMClient * restrict c)
{
#if LOG_LEVEL > 4
	JBWM_LOG("mark_removal(): ignore_unmap is %d", c->ignore_unmap);
#endif//LOG_LEVEL
	c->opt.remove = events_need_cleanup = (c->ignore_unmap--<1);
}
static void handle_colormap_notify(struct JBWMClient * restrict c,
	XColormapEvent * e)
{
	JBWM_LOG("ColormapNotify");
	if (c && e->new)
		XInstallColormap(e->display, c->cmap = e->colormap);
}
void jbwm_events_loop(Display * d)
{
	for (;;) {
		XEvent ev;
		XNextEvent(d, &ev);
		struct JBWMClient * c = jbwm_get_client(ev.xany.window);
		switch (ev.type) {
		case ButtonRelease:
		case KeyRelease:
		case MapNotify:
		case MappingNotify:
		case MotionNotify:
			// ignore
			break;
		case ConfigureNotify:
			if (c && !ev.xconfigure.override_redirect)
				jbwm_move_resize(d, c);
			break;
		case ConfigureRequest:
			handle_configure_request(&ev.xconfigurerequest);
			XSync(d, false);
			if (c)
				jbwm_move_resize(d, c);
			break;
		case ReparentNotify:
			ELOG("ReparentNotify");
			/* Reset last_window to allow other clients
			 * with the same window id to be started.  */
			last_window = 0;
			break;
		case KeyPress:
			ELOG("KeyPress");
			jbwm_handle_key_event(&ev.xkey);
			break;
		case ButtonPress:
			ELOG("ButtonPress");
			if(c)
				jbwm_handle_button_event(&ev.xbutton, c);
			break;
		case EnterNotify:
			if (c && ev.xcrossing.window == c->parent)
				jbwm_select_client(d, c);
			break;
#ifdef JBWM_USE_TITLE_BAR
		case Expose:
			if (c && !ev.xexpose.count)
				jbwm_update_title_bar(d, c);
			break;
#endif//JBWM_USE_TITLE_BAR
#ifdef JBWM_USE_EWMH
		case CreateNotify:
			ELOG("CreateNotify");
			if (ev.xcreatewindow.override_redirect) // internal
				jbwm_ewmh_update_client_list(d);
			break;
		case DestroyNotify:
			ELOG("DestroyNotify");
			if (!c) // only bother if event was not on a client
				jbwm_ewmh_update_client_list(d);
			break;
#endif//JBWM_USE_EWMH
		case UnmapNotify:
			ELOG("UnmapNotify");
			if (c)
				mark_removal(c);
			break;
		case MapRequest:
			if (!c)
				handle_map_request(&ev.xmaprequest);
			break;
		case PropertyNotify:
			if (c)
				handle_property_change(&ev.xproperty, c);
			break;
		case ColormapNotify:
			handle_colormap_notify(c, &ev.xcolormap);
			break;
#ifdef JBWM_USE_EWMH
		case ClientMessage:
			jbwm_ewmh_handle_client_message(&ev.xclient, c);
			break;
#endif//JBWM_USE_EWMH
#if LOG_LEVEL > 3
		default:
			JBWM_LOG("Unhandled event %d", ev.type);
#endif//LOG_LEVEL
		}
		if (events_need_cleanup) {
			cleanup(d, jbwm_get_head_client());
			events_need_cleanup = false;
		}
	}
}

