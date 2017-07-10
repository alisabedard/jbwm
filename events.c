// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "events.h"
#include <X11/Xatom.h>
#include "button_event.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_client.h"
#include "key_event.h"
#include "log.h"
#include "move_resize.h"
#include "new.h"
#include "screen.h"
#include "select.h"
#include "title_bar.h"
#include "util.h"
#include "wm_state.h"
// Set log level for events
#define JBWM_LOG_EVENTS 5
static bool events_need_cleanup;
static struct JBWMScreen * get_screen(const Window root, const int i)
{
	struct JBWMScreen * s = jbwm_get_screens();
	return RootWindowOfScreen(s[i].xlib) == root ? s
		: get_screen(root, i + 1);
}
static void cleanup(Display * d, struct JBWMClient * i)
{
	JBWM_LOG("cleanup");
	if (!i)
		return;
	struct JBWMClient * next = i->next; // save
	if (i->opt.remove)
		jbwm_client_free(i);
	cleanup(d, next);
}
static void handle_property_change(XPropertyEvent * e,
	struct JBWMClient * restrict c)
{
	if (e->state != PropertyNewValue)
		return;
	if (e->atom == XA_WM_NAME)
		jbwm_update_title_bar(c);
	else {
		Display * d = e->display;
#if JBWM_LOG_EVENTS > 3
		jbwm_print_atom(d, e->atom, __FILE__, __LINE__);
#endif//JBWM_LOG_EVENTS > 3
		if (e->atom == jbwm_get_wm_state(d))
			jbwm_move_resize(c);
	}
}
static void handle_configure_request(XConfigureRequestEvent * e)
{
	JBWM_LOG("handle_configure_request():"
		"x: %d, y: %d, w: %d, h: %d, b: %d",
		e->x, e->y, e->width, e->height, e->border_width);
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
	static unsigned long serial;
	if (e->serial == serial)
		return;
	serial = e->serial;
	JBWM_LOG("handle_map_request(), send_event:%d", e->send_event);
	jbwm_new_client(e->display, get_screen(e->parent, 0), e->window);
}
static inline void mark_removal(struct JBWMClient * restrict c)
{
	JBWM_LOG("mark_removal(): ignore_unmap is %d", c->ignore_unmap);
	c->opt.remove = events_need_cleanup = (c->ignore_unmap--<1);
}
static void handle_colormap_notify(struct JBWMClient * restrict c,
	XColormapEvent * e)
{
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
#if defined(JBWM_LOG_EVENTS) && JBWM_LOG_EVENTS > 0
#define ELOG(level, ename) if (JBWM_LOG_EVENTS >= level) {\
	JBWM_LOG("\tXEVENT: %s", ename);}
#else//!JBWM_LOG_EVENTS
#define ELOG(level, ename)
#endif//JBWM_LOG_EVENTS
		case ButtonRelease:
		case KeyRelease:
		case MapNotify:
		case MappingNotify:
		case MotionNotify:
		case ReparentNotify:
			// ignore
			break;
		case ConfigureNotify:
			ELOG(7, "ConfigureNotify");
			if (c && !ev.xconfigure.override_redirect)
				jbwm_move_resize(c);
			break;
		case ConfigureRequest:
			ELOG(4, "ConfigureRequest");
			handle_configure_request(&ev.xconfigurerequest);
			XSync(d, false);
			if (c)
				jbwm_move_resize(c);
			break;
		case KeyPress:
			ELOG(3, "KeyPress");
			jbwm_handle_key_event(&ev.xkey);
			break;
		case ButtonPress:
			ELOG(4, "ButtonPress");
			if(c)
				jbwm_handle_button_event(&ev.xbutton, c);
			break;
		case EnterNotify:
			ELOG(6, "EnterNotify");
			if (c && ev.xcrossing.window == c->parent)
				jbwm_select_client(c);
			break;
		case Expose:
			ELOG(9, "Expose");
			if (c && !ev.xexpose.count)
				jbwm_update_title_bar(c);
			break;
		case CreateNotify:
			ELOG(3, "CreateNotify")
			if (ev.xcreatewindow.override_redirect) // internal
				jbwm_ewmh_update_client_list(d);
			break;
		case DestroyNotify:
			ELOG(3, "DestroyNotify");
			if (!c) // only bother if event was not on a client
				jbwm_ewmh_update_client_list(d);
			break;
		case UnmapNotify:
			ELOG(3, "UnmapNotify");
			if (c)
				mark_removal(c);
			break;
		case MapRequest:
			ELOG(3, "MapRequest");
			if (!c)
				handle_map_request(&ev.xmaprequest);
			break;
		case PropertyNotify:
			ELOG(8, "PropertyNotify");
			if (c)
				handle_property_change(&ev.xproperty, c);
			break;
		case ColormapNotify:
			ELOG(5, "ColormapNotify");
			handle_colormap_notify(c, &ev.xcolormap);
			break;
		case ClientMessage:
			ELOG(3, "ClientMessage");
			jbwm_ewmh_handle_client_message(&ev.xclient, c);
			break;
#undef ELOG
#ifdef DEBUG
		default:
			JBWM_LOG("Unhandled event %d", ev.type);
#endif//DEBUG
		}
		if (events_need_cleanup) {
			cleanup(d, jbwm_get_head_client());
			events_need_cleanup = false;
		}
	}
}

