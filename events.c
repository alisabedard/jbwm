// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "events.h"

#include "button_event.h"
#include "client.h"
#include "ewmh.h"
#include "jbwmenv.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include "screen.h"
#include "titlebar.h"
#ifdef DEBUG
#include "util.h"
#endif//DEBUG

#include <stdlib.h>
#include <stdnoreturn.h>
#include <X11/Xatom.h>

static ScreenInfo *find_screen(const Window root)
{
	uint8_t i = ScreenCount(jbwm.dpy);

	while (i--)
		if (jbwm.screens[i].root == root)
			return &jbwm.screens[i];

	return NULL;
}

static void relink_window_list(Client * c)
{
	LOG("relink_window_list");

	if (jbwm.head == c)
		jbwm.head = c->next;
	else
		for (Client * p = jbwm.head; p && p->next; p = p->next)
			if (p->next == c) {
				p->next = c->next;
				return;
			}
}

static void unparent_window(Client * c)
{
	LOG("unparent_window");
	XReparentWindow(jbwm.dpy, c->window, c->screen->root,
		c->size.x, c->size.y);
	XRemoveFromSaveSet(jbwm.dpy, c->window);

	if (c->parent)
		XDestroyWindow(jbwm.dpy, c->parent);

	c->parent = 0;
}

static void cleanup(void)
{
	LOG("cleanup()");
	jbwm.need_cleanup = false;

	for (Client * i, * c = jbwm.head; c; c = i) {
		//Client *i = c->next;
		i = c->next;
		if (c->opt.remove) {
#ifdef EWMH
			XDeleteProperty(jbwm.dpy, c->window, ewmh[WM_DESKTOP]);
			XDeleteProperty(jbwm.dpy, c->window, ewmh[WM_STATE]);
#endif//EWMH
			set_wm_state(c, WithdrawnState);
			unparent_window(c);
			relink_window_list(c);

			if (jbwm.current == c)
				jbwm.current = NULL;

			free(c);
		}
		if (!i) return;
	}
}

static void handle_unmap_event(XUnmapEvent * e)
{
	Client *c = find_client(e->window);

	if (!c) return;

	LOG("handle_unmap_event(e): %d ignores remaining", c->ignore_unmap);

	if (c->ignore_unmap < 1) {
		LOG("!c->ignore_unmap");
		c->opt.remove = true;
		jbwm.need_cleanup = true;
	} else
		c->ignore_unmap--;
}

static void handle_colormap_change(XColormapEvent * e)
{
	LOG("handle_colormap_change(e)");
	Client *c = find_client(e->window);

	if (c && e->new) {
		c->cmap = e->colormap;
		XInstallColormap(jbwm.dpy, c->cmap);
	}
}

static void handle_wm_hints(Client * c)
{
	LOG("handle_wm_hints");
	XWMHints *h = XGetWMHints(jbwm.dpy, c->window);

	if (h->flags & XUrgencyHint) {
		switch_vdesk(c->screen, c->screen->vdesk);
		unhide(c);
		XRaiseWindow(jbwm.dpy, c->parent);
	}

	XFree(h);
}

static void handle_property_change(XPropertyEvent * e)
{
	Client *c = find_client(e->window);

	if (!c) return;

	const Atom a = e->atom;

#ifdef DEBUG
		print_atom(a, __LINE__);
#endif//DEBUG

	if (a == XA_WM_HINTS)
		handle_wm_hints(c);

#ifdef USE_TBAR
	else if (a == XA_WM_NAME)
		update_titlebar(c);
	else
		  moveresize(c);
#endif//USE_TBAR
}

static void handle_enter_event(XCrossingEvent * restrict e)
{
	Client * c = find_client(e->window);
	if(c) {
		/* Only deal with the parent window, prevent multiple enter
		 * events.  */
		if(e->window != c->parent)
			return;
		select_client(c);
	}
}

#ifdef USE_TBAR
static void handle_expose_event(XEvent * ev)
{
	// Ignore extra expose events
	if (ev->xexpose.count == 0) {
		LOG("handle_expose_event");
		const Window w = ev->xexpose.window;
		Client *c = find_client(w);

		if (!c)
			return;

		if (w == c->titlebar)
			update_titlebar(c);
	}
}
#endif//USE_TBAR

static void handle_configure_request(XConfigureRequestEvent * e)
{
	XConfigureWindow(jbwm.dpy, e->window, e->value_mask, 
		&(XWindowChanges){ .x = e->x, .y = e->y,
		.width = e->width, .height = e->height,
		.border_width = e->border_width,
		.sibling = e->above, .stack_mode = e->detail});
}

noreturn void main_event_loop(void)
{
	XEvent ev;
 head:
	XNextEvent(jbwm.dpy, &ev);

	switch (ev.type) {
	case EnterNotify:
		handle_enter_event(&ev.xcrossing);
		break;

	case UnmapNotify:
		handle_unmap_event(&ev.xunmap);
		break;

	case PropertyNotify:
		handle_property_change(&ev.xproperty);
		break;

	case MapRequest:
		if (!find_client(ev.xmaprequest.window))
			make_new_client(ev.xmaprequest.window,
					find_screen(ev.xmaprequest.parent));

		break;

	case KeyPress:
		jbwm_handle_key_event(&ev.xkey);
		break;
#ifdef USE_TBAR

	case Expose:
		handle_expose_event(&ev);
		break;
#endif

	case ButtonPress:
		jbwm_handle_button_event(&ev.xbutton);
		break;

	case ConfigureRequest:
		handle_configure_request(&ev.xconfigurerequest);
		break;

	case ConfigureNotify:
		if (!ev.xconfigure.override_redirect) {
			Client *c = find_client(ev.xconfigure.window);

			if (c)
				moveresize(c);
		}

		break;

	case ColormapNotify:
		handle_colormap_change(&ev.xcolormap);
		break;

#ifdef EWMH
	case CreateNotify:
	case DestroyNotify:
		ewmh_update_client_list();
		break;

	case ClientMessage:
		ewmh_client_message(&ev.xclient);
		break;
#endif//EWMH

#if defined(DEBUG) && defined(XDEBUG)

	case MapNotify:
		LOG("MapNotify");
		break;

	case MappingNotify:
		LOG("MappingNotify");
		break;

	case KeyRelease:
		LOG("KeyRelease");
		break;

	case ReparentNotify:
		LOG("ReparentNotify");
		break;

	case ButtonRelease:
		LOG("ButtonRelease");
		break;

	default:
		LOG("Unhandled event (%d)", ev.type);
#endif//DEBUG&&XDEBUG
	}

	if (jbwm.need_cleanup)
		cleanup();

	goto head;
}
