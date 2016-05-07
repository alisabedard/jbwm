// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "events.h"

#include "button_event.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "jbwmenv.h"
#include "keys.h"
#include "log.h"
#include "mwm.h"
#include "new.h"
#include "screen.h"
#include "titlebar.h"
#include "util.h"

#include <stdlib.h>
#include <X11/Xatom.h>

__attribute__((pure)) // Return does not change between calls
static ScreenInfo *find_screen(const Window root)
{
	uint8_t i = ScreenCount(jbwm.dpy);

	while (i--)
		if (jbwm.screens[i].root == root)
			return &jbwm.screens[i];

	return NULL;
}

// Relink c's linked list to exclude c
static void relink_window_list(Client * c)
{
	LOG("relink_window_list");
	if (jbwm.current == c) // Remove selection target
		jbwm.current = NULL;
	if (jbwm.head == c) {
		jbwm.head = c->next;
		return;
	}
	for (Client * p = jbwm.head; p && p->next; p = p->next) {
		if (p->next == c) { // Close the link
			p->next = c->next;
			return;
		}
	}
}

static void cleanup(void)
{
	LOG("cleanup");
	jbwm.need_cleanup = false;
	for(Client * i, * c = jbwm.head; c; c=i) {
		i=c->next;
		if(c->opt.remove) {
			XReparentWindow(jbwm.dpy, c->window,
				c->screen->root,
				c->size.x, c->size.y);
			XRemoveFromSaveSet(jbwm.dpy, c->window);
			XDestroyWindow(jbwm.dpy, c->parent);
			relink_window_list(c);
			free(c);
		}
		if(!i) return;
	}
}

static void handle_wm_hints(Client * c)
{
	LOG("handle_wm_hints");
	XWMHints *h = XGetWMHints(jbwm.dpy, c->window);
	if(h) {
		if (h->flags & XUrgencyHint) {
			switch_vdesk(c->screen, c->vdesk);
			XRaiseWindow(jbwm.dpy, c->parent);
		}

		XFree(h);
	}
}

static void handle_property_change(XPropertyEvent * restrict e,
	Client * restrict c)
{
	LOG("handle_property_change");
	print_atom(e->atom, __LINE__);
	if(e->state != PropertyNewValue)
		  return;
	switch(e->atom) {
#ifdef USE_TBAR
	case XA_WM_NAME:
		update_titlebar(c);
		break;
#endif//USE_TBAR
	case XA_WM_HINTS:
		handle_wm_hints(c);
		break;
	default:
		moveresize(c);
	}
}

static void handle_configure_request(XConfigureRequestEvent * e)
{
	LOG("handle_configure_request");
	XConfigureWindow(jbwm.dpy, e->window, e->value_mask,
		&(XWindowChanges){ .x = e->x, .y = e->y,
		.width = e->width, .height = e->height,
		.border_width = e->border_width,
		.sibling = e->above, .stack_mode = e->detail});
}

void main_event_loop(void)
{
	XEvent ev;
	Client * c;
 head:
	XNextEvent(jbwm.dpy, &ev);
	c=find_client(ev.xany.window);
	switch (ev.type) {
	case EnterNotify:
		if(c && (ev.xcrossing.window == c->parent))
			  select_client(c);
		break;

	case UnmapNotify:
		if(c) {
			LOG("UnmapNotify: ignore_unmap is %d",
				c->ignore_unmap);
			c->opt.remove=jbwm.need_cleanup=(c->ignore_unmap<1);
			c->ignore_unmap--; // Decrement here!
		}
		break;

	case PropertyNotify:
		if (c) handle_property_change(&ev.xproperty, c);
		break;

	case MapRequest:
		if (!c) make_new_client(ev.xmaprequest.window,
			find_screen(ev.xmaprequest.parent));
		break;

	case KeyPress:
		jbwm_handle_key_event(&ev.xkey);
		break;
#ifdef USE_TBAR
	case Expose:
		if (c && !ev.xexpose.count)
			  update_titlebar(c);
		break;
#endif//USE_TBAR

	case ButtonPress:
		if(c) jbwm_handle_button_event(&ev.xbutton, c);
		break;

	case ConfigureRequest:
		handle_configure_request(&ev.xconfigurerequest);
		break;

	case ColormapNotify:
		if (c && ev.xcolormap.new) {
			LOG("ColormapNotify");
			c->cmap = ev.xcolormap.colormap;
			XInstallColormap(jbwm.dpy, c->cmap);
		}
		break;

#ifdef EWMH
	case CreateNotify:
		LOG("CreateNotify");
	case DestroyNotify:
		LOG("->>DestroyNotify");
		ewmh_update_client_list();
		break;

	case ClientMessage:
		ewmh_client_message(&ev.xclient, c);
		break;
#endif//EWMH

#ifdef DEBUG
	case ConfigureNotify: LOG("ConfigureNotify"); break;
	case MapNotify: LOG("MapNotify"); break;
	case MappingNotify: LOG("MappingNotify"); break;
	case MotionNotify: LOG("MotionNotify"); break;
	case KeyRelease: LOG("KeyRelease"); break;
	case ReparentNotify: LOG("ReparentNotify"); break;
	case ButtonRelease: LOG("ButtonRelease"); break;
#endif//DEBUG
	default:
		LOG("Unhandled event (%d)", ev.type);
	}
	if (jbwm.need_cleanup) cleanup();
	goto head;
}
