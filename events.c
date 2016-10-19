// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "events.h"
#include "button_event.h"
#include "client.h"
#include "ewmh_state.h"
#include "ewmh.h"
#include "JBWMEnv.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include "screen.h"
#include "titlebar.h"
#include "util.h"
#include <stdlib.h>
#include <X11/Xatom.h>
__attribute__((pure))
static struct JBWMScreen * get_screen(const int8_t i,
	const jbwm_window_t root)
{
	return jbwm.s[i].root == root ? &jbwm.s[i]
		: get_screen(i - 1, root);
}
// Relink c's linked list to exclude c
static void relink_window_list(struct JBWMClient * c)
{
	LOG("relink_window_list");
	if (jbwm.current == c) // Remove selection target
		jbwm.current = NULL;
	if (jbwm.head == c) {
		jbwm.head = c->next;
		return;
	}
	for (struct JBWMClient * p = jbwm.head; p && p->next; p = p->next) {
		if (p->next == c) { // Close the link
			p->next = c->next;
			return;
		}
	}
}
void jbwm_free_client(struct JBWMClient * restrict c)
{
	const jbwm_window_t w = c->window;
#ifdef JBWM_USE_EWMH
	// Per ICCCM + JBWM_USE_EWMH:
	XDeleteProperty(jbwm.d, w, ewmh[WM_STATE]);
	XDeleteProperty(jbwm.d, w, ewmh[WM_DESKTOP]);
#endif//JBWM_USE_EWMH
	XReparentWindow(jbwm.d, w, c->screen->root, c->size.x, c->size.y);
	XRemoveFromSaveSet(jbwm.d, w);
	if(c->parent)
		XDestroyWindow(jbwm.d, c->parent);
	relink_window_list(c);
	free(c);
	jbwm.last = 0; /* allow this client's window id to be reused
			  for another client.  */
}
static void cleanup(void)
{
	LOG("cleanup");
	jbwm.need_cleanup = false;
	struct JBWMClient * c = jbwm.head;
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
#ifdef EVENT_DEBUG
	jbwm_print_atom(e->atom, __FILE__, __LINE__);
#endif//EVENT_DEBUG
	if(e->state != PropertyNewValue)
		  return;
	if (e->atom == XA_WM_NAME)
		jbwm_update_titlebar(c);
	else {
		if (e->atom == jbwm_get_wm_state())
			jbwm_move_resize(c);
		jbwm_print_atom(e->atom, __FILE__, __LINE__);
	}
}
static void handle_configure_request(XConfigureRequestEvent * e)
{
	LOG("handle_configure_request");
	XConfigureWindow(jbwm.d, e->window, e->value_mask,
		&(XWindowChanges){ .x = e->x, .y = e->y,
		.width = e->width, .height = e->height,
		.border_width = e->border_width,
		.sibling = e->above, .stack_mode = e->detail});
}
static void iteration(void)
{
	XEvent ev;
	struct JBWMClient * c;
	XNextEvent(jbwm.d, &ev);
	c = jbwm_get_client(ev.xany.window);
	switch (ev.type) {
	case KeyPress:
		jbwm_handle_key_event(&ev.xkey);
		break;
	case ButtonPress:
		if(c) jbwm_handle_button_event(&ev.xbutton, c);
		break;
	case EnterNotify:
		if(c && (ev.xcrossing.window == c->parent))
			  jbwm_select_client(c);
		break;
#ifdef USE_TBAR
	case Expose:
		if (c && !ev.xexpose.count)
			  jbwm_update_titlebar(c);
		break;
#endif//USE_TBAR
#ifdef JBWM_USE_EWMH
	case CreateNotify:
	case DestroyNotify:
		jbwm_ewmh_update_client_list();
		break;
#endif//JBWM_USE_EWMH
	case UnmapNotify:
		if(!c)
			  break;
		LOG("UnmapNotify: ignore_unmap is %d", c->ignore_unmap);
		c->opt.remove=jbwm.need_cleanup=(c->ignore_unmap--<1);
		break;
	case MapRequest:
		LOG("MapRequest, send_event:%d", ev.xmaprequest.send_event);
		/* This check fixes a race condition in libreoffice dialogs,
		   where an attempt is made to request mapping twice.  */
		if(c || ev.xmaprequest.window == jbwm.last)
			break;
		jbwm.last = ev.xmaprequest.window;
		jbwm_new_client(ev.xmaprequest.window, get_screen(
			ScreenCount(jbwm.d), ev.xmaprequest.parent));
		break;
	case ConfigureRequest:
		handle_configure_request(&ev.xconfigurerequest);
		break;
	case PropertyNotify:
		if (c) handle_property_change(&ev.xproperty, c);
		break;
	case ColormapNotify:
		if (c && ev.xcolormap.new) {
			LOG("ColormapNotify");
			c->cmap = ev.xcolormap.colormap;
			XInstallColormap(jbwm.d, c->cmap);
		}
		break;
#ifdef JBWM_USE_EWMH
	case ClientMessage:
		jbwm_ewmh_handle_client_message(&ev.xclient, c);
		break;
#endif//JBWM_USE_EWMH
#ifdef EVENT_DEBUG
	default:
		LOG("Unhandled event (%d)", ev.type);
#endif//EVENT_DEBUG
	}
	if (jbwm.need_cleanup) {
		cleanup();
		jbwm.last=0; // Fix ignoring every other new window
	}
}
void jbwm_event_loop(void)
{
	for(;;)
		iteration();
}
