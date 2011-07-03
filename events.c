/* arwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <arwm@6809.org.uk>
 * see README for license and other details. */

#include <stdlib.h>
#include "arwm.h"
#include "log.h"

void 
arwm_current_to_head(void) 
{
	if (current && (current != head_client)) 
	{
		Client *c;

		/* Advance to either end of list or current node.  */
		for (c = head_client; c && (c->next != current) ; c = c->next)
			;
		if(c && (c->next == current))
		{
			c->next = current->next;
			current->next = head_client;
			head_client = current;
		}
	}
}

static void 
handle_map_request(XMapRequestEvent *e) 
{
	Client *c = find_client(e->window);
	LOG_DEBUG("handle_map_request()\n");
	if (c) 
	{
		if (c->vdesk != c->screen->vdesk)
			switch_vdesk(c->screen, c->vdesk);
		unhide(c, RAISE);
	} 
	else 
	{
		XWindowAttributes attr;
		LOG_DEBUG("handle_map_request() : don't know this window,"
			" calling make_new_client();\n");
		XGetWindowAttributes(arwm.X.dpy, e->window, &attr);
		LOG_DEBUG("XGWA():w:\t%d\th:%d\n",attr.width,attr.height);
		make_new_client(e->window, find_screen(attr.root));
	}
}

static void 
handle_unmap_event(XUnmapEvent *e) 
{
	Client *c = find_client(e->window);

	LOG_DEBUG("handle_unmap_event():  ");
	if (c) 
	{
		if (c->ignore_unmap) 
		{
			c->ignore_unmap--;
			LOG_DEBUG("ignored (%d ignores remaining)\n", 
				c->ignore_unmap);
		} 
		else 
		{
			LOG_DEBUG("flagging client for removal\n");
			c->flags |= AR_CLIENT_REMOVE;
			need_client_tidy = 1;
		}
	} 
}

static void 
handle_colormap_change(XColormapEvent *e) {
	Client *c = find_client(e->window);
	MARK;
	if (c && e->new) {
		c->cmap = e->colormap;
		XInstallColormap(arwm.X.dpy, c->cmap);
	}
}

static void 
handle_property_change(XPropertyEvent *e) 
{
	Atom atom = e->atom;
	Client *c;

	/* Filter extraneous atoms to improve performance.  */
        switch(atom)
          {
          case 221:
          case 309:
          case 37:
            return;
          default:
            break;
          }

	LOG_DEBUG("handle_property_change(e);\n");
	c=find_client(e->window);
	if (c)
	{

		LOG_DEBUG("\tProperty atom:  %d\n", (int)atom);
		if (atom == XA_WM_NORMAL_HINTS) 
		{
			LOG_DEBUG("\tget_wm_normal_hints(c);\n");
			get_wm_normal_hints(c);
		}
		else if(atom==XA_WM_NAME)
			update_info_window(c);
		else 
			arwm_handle_net_wm_state_hints(c);
	}
}

static void 
handle_enter_event(XCrossingEvent *e) 
{
	Client *c;

	if ((c = find_client(e->window))) 
	{
		/* The first check insures that the window which
		   received the enter event is on the current virtual
		   desktop.  The second check isolates the event to
		   only the application window in order to avoid
		   extraneous enter event processing.  */
		if((c->vdesk != c->screen->vdesk) 
                   || (e->window != c->window) 
                   || (current==c))
			return;
		select_client(c);
		arwm_current_to_head();
	}
}

static void 
handle_mappingnotify_event(XMappingEvent *e) 
{
	LOG_DEBUG("handle_mappingnotify_event\n");
	XRefreshKeyboardMapping(e);
	if (e->request == MappingKeyboard) 
	{
		int i;
		for (i = 0; i < arwm.X.num_screens; i++) 
			grab_keys_for_screen(&arwm.X.screens[i]);
	}
}

static void 
handle_shape_event(XEvent *xe) 
{

	Client * c;

	c = find_client(xe->xany.window);
	if (c) set_shape(c);
}

#ifdef TITLEBAR
static void
handle_expose_event(XEvent * ev)
{
	if(ev->xexpose.count==0)
	{
		/* xproperty was used instead of xexpose, previously.  */
		Window ev_win = ev->xexpose.window;
		Client * c = find_client(ev_win);

		LOG_DEBUG("handle_expose_event(ev);\n");

		if(c && (ev_win == c->info_window))
			update_info_window(c);
	}
}
#endif /* TITLEBAR */

void 
arwm_process_events(void);

void 
event_main_loop(void)
{
	/* main event loop here */
	for(;;) {
		arwm_process_events();
		if (need_client_tidy) 
		{
			Client *c, *nc;
			for (c = head_client; c; c = nc) 
			{
				nc = c->next;
				if (c->flags & AR_CLIENT_REMOVE)
					remove_client(c);
			}
		}
	}
}

void 
arwm_process_events(void)
{
	XEvent ev;

	XNextEvent(arwm.X.dpy, &ev);
	LOG_XDEBUG("Event ID: %d\n", ev.type);
	switch (ev.type) 
	{
	case NoExpose:
		LOG_DEBUG("event_main_loop():  NoExpose\n");
		break;
#ifdef TITLEBAR
	case Expose:
		handle_expose_event(&ev);
		break;
#endif /* TITLEBAR */
	case KeyPress:
		arwm_handle_key_event(&ev.xkey); 
		break;
	case ButtonPress:
		arwm_handle_button_event(&ev.xbutton); 
		break;
	case ConfigureRequest:
		LOG_DEBUG("event_main_loop(): ConfigureRequest\n");
		arwm_handle_configure_request(&ev.xconfigurerequest); 
		break;
	case MapRequest:
		LOG_DEBUG("event_main_loop():  MapRequest\n");
		handle_map_request(&ev.xmaprequest); 
		break;
	case ColormapNotify:
		handle_colormap_change(&ev.xcolormap); 
		break;
	case EnterNotify:
		handle_enter_event(&ev.xcrossing); 
		break;
	case PropertyNotify:
		handle_property_change(&ev.xproperty); 
		break;
	case UnmapNotify:
		handle_unmap_event(&ev.xunmap); 
		break;
	case MapNotify:
		break;
	case MappingNotify:
		handle_mappingnotify_event(&ev.xmapping); 
		break;
	default:
		if (arwm.X.have_shape && ev.type == arwm.X.shape_event) 
			handle_shape_event(&ev);
	}
}

