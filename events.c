// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "events.h"
#include "button_event.h"
#include "client.h"
#include "ewmh_state.h"
#include "ewmh.h"
#include "jbwm.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include "screen.h"
#include "title_bar.h"
#include "util.h"
#include <stdlib.h>
#include <X11/Xatom.h>
static struct {
	jbwm_window_t last_window;
	bool need_cleanup;
} jbwm_events_data;
__attribute__((pure))
static struct JBWMScreen * get_screen(const int8_t i,
	const jbwm_window_t root)
{
	struct JBWMScreen * s = jbwm_get_screens();
	return s[i].root == root ? s + i
		: get_screen(i - 1, root);
}
void jbwm_free_client(struct JBWMClient * restrict c)
{
	const jbwm_window_t w = c->window;
	Display * restrict d = jbwm_get_display();
#ifdef JBWM_USE_EWMH
	// Per ICCCM + JBWM_USE_EWMH:
	XDeleteProperty(d, w, ewmh[WM_STATE]);
	XDeleteProperty(d, w, ewmh[WM_DESKTOP]);
#endif//JBWM_USE_EWMH
	XReparentWindow(d, w, jbwm_get_screens()[c->screen].root,
		c->size.x, c->size.y);
	XRemoveFromSaveSet(d, w);
	if(c->parent)
		XDestroyWindow(d, c->parent);
	jbwm_relink_client_list(c);
	free(c);
	/* Allow this client's window id to be reused for another client: */
	jbwm_events_data.last_window = 0;
}
static void cleanup(void)
{
	JBWM_LOG("cleanup");
	jbwm_events_data.need_cleanup = false;
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
#ifdef EVENT_DEBUG
	jbwm_print_atom(e->atom, __FILE__, __LINE__);
#endif//EVENT_DEBUG
	if(e->state != PropertyNewValue)
		  return;
	if (e->atom == XA_WM_NAME)
		jbwm_update_title_bar(e->display, c);
	else {
		if (e->atom == jbwm_get_wm_state())
			jbwm_move_resize(c);
		jbwm_print_atom(e->display, e->atom,
			__FILE__, __LINE__);
	}
}
static void handle_configure_request(XConfigureRequestEvent * restrict e)
{
	JBWM_LOG("handle_configure_request");
	XConfigureWindow(e->display, e->window, e->value_mask,
		&(XWindowChanges){ .x = e->x, .y = e->y,
		.width = e->width, .height = e->height,
		.border_width = e->border_width,
		.sibling = e->above, .stack_mode = e->detail});
}
static void handle_map_request(XMapRequestEvent * restrict e)
{
	JBWM_LOG("MapRequest, send_event:%d", e->send_event);
	/* This check fixes a race condition in libreoffice dialogs,
	   where an attempt is made to request mapping twice.  */
	if(e->window == jbwm_events_data.last_window)
		return;
	jbwm_events_data.last_window = e->window;
	Display * restrict d = e->display;
	jbwm_new_client(d, get_screen(ScreenCount(d), e->parent), e->window);

}
static void iteration(void)
{
	XEvent ev;
	XNextEvent(jbwm_get_display(), &ev);
	struct JBWMClient * restrict c = jbwm_get_client(ev.xany.window);
	switch (ev.type) {
	case KeyPress:
		jbwm_handle_key_event(ev.xkey.display, &ev.xkey);
		break;
	case ButtonPress:
		if(c)
			jbwm_handle_button_event(ev.xbutton.display,
				&ev.xbutton, c);
		break;
	case EnterNotify:
		if(c && (ev.xcrossing.window == c->parent))
			  jbwm_select_client(c);
		break;
#ifdef JBWM_USE_TITLE_BAR
	case Expose:
		if (c && !ev.xexpose.count)
			  jbwm_update_title_bar(ev.xexpose.display, c);
		break;
#endif//JBWM_USE_TITLE_BAR
#ifdef JBWM_USE_EWMH
	case CreateNotify:
	case DestroyNotify:
		jbwm_ewmh_update_client_list();
		break;
#endif//JBWM_USE_EWMH
	case UnmapNotify:
		if(!c)
			  break;
		JBWM_LOG("UnmapNotify: ignore_unmap is %d", c->ignore_unmap);
		c->opt.remove=jbwm_events_data.need_cleanup
			= (c->ignore_unmap--<1);
		break;
	case MapRequest:
		if (!c)
			handle_map_request(&ev.xmaprequest);
		break;
	case ConfigureRequest:
		handle_configure_request(&ev.xconfigurerequest);
		break;
	case PropertyNotify:
		if (c)
			handle_property_change(&ev.xproperty, c);
		break;
	case ColormapNotify:
		if (c && ev.xcolormap.new) {
			JBWM_LOG("ColormapNotify");
			c->cmap = ev.xcolormap.colormap;
			XInstallColormap(ev.xcolormap.display, c->cmap);
		}
		break;
#ifdef JBWM_USE_EWMH
	case ClientMessage:
		jbwm_ewmh_handle_client_message(&ev.xclient, c);
		break;
#endif//JBWM_USE_EWMH
#ifdef EVENT_DEBUG
	default:
		JBWM_LOG("Unhandled event (%d)", ev.type);
#endif//EVENT_DEBUG
	}
	if (jbwm_events_data.need_cleanup) {
		cleanup();
		// Fix ignoring every other new window:
		jbwm_events_data.last_window=0;
	}
}
void jbwm_event_loop(void)
{
	for(;;)
		iteration();
}
