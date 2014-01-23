/* arwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <arwm@6809.org.uk>
 * see README for license and other details. */

#include <stdlib.h>
#include "arwm.h"

void
arwm_current_to_head(void)
{
	if(current && (current != head_client))
	{
		Client *c;

		/* Advance to either end of list or current node.  */
		for(c = head_client; c && (c->next != current);
			c = c->next)
			;
		if(c && (c->next == current))
		{
			c->next = current->next;
			current->next = head_client;
			head_client = current;
		}
	}
}

static ScreenInfo *
find_screen(Window root)
{
	ubyte i;

	for(i = 0; i < arwm.X.num_screens; i++)
		if(arwm.X.screens[i].root == root)
			return &arwm.X.screens[i];

	return NULL;
}

static void
handle_map_request(XMapRequestEvent * e)
{
	Client *c = find_client(e->window);
	
	if(c)
	{
		if(c->vdesk != c->screen->vdesk)
			switch_vdesk(c->screen, c->vdesk);
		unhide(c, RAISE);
		c->ignore_unmap++;
	}
	else
	{
		XWindowAttributes attr;

		XGetWindowAttributes(arwm.X.dpy, e->window, &attr);
		make_new_client(e->window, find_screen(attr.root));
	}
}
static void
handle_unmap_event(XUnmapEvent * e)
{
	Client *c = find_client(e->window);
	if(c)
	{
		if(c->ignore_unmap>0)
			c->ignore_unmap--;
		else
		{
			c->flags |= AR_CLIENT_REMOVE;
			arwm.flags |= ARWM_FLAG_NEED_TIDY;
		}
	}
}

#ifdef USE_CMAP
static void
handle_colormap_change(XColormapEvent * e)
{
	Client *c = find_client(e->window);

	if(c && e->new)
	{
		c->cmap = e->colormap;
		XInstallColormap(arwm.X.dpy, c->cmap);
	}
}
#endif /* USE_CMAP */

static void
handle_property_change(XPropertyEvent * e)
{
	Client *c;

	const Atom atom = e->atom;

	c = find_client(e->window);
	if(c)
	{
		moveresize(c);
		if(atom == XA_WM_NORMAL_HINTS)
			get_wm_normal_hints(c);
#ifdef USE_TBAR
		else if(atom == XA_WM_NAME)
			update_info_window(c);
#endif
	}
}

static void
handle_enter_event(XCrossingEvent * e)
{
	Client *c;

	if((c = find_client(e->window)))
	{
		/* The first check insures that the window which
		   received the enter event is on the current virtual
		   desktop.  The second check isolates the event to
		   only the application window in order to avoid
		   extraneous enter event processing.  */
		if((c->vdesk != c->screen->vdesk)
			|| (e->window != c->window))
			return;
		select_client(c);
		arwm_current_to_head();
	}
	else
		XUnmapWindow(arwm.X.dpy, e->window);
}

#if 0
static void
handle_mappingnotify_event(XMappingEvent * e)
{
	LOG("handle_mappingnotify_event\n");
	XRefreshKeyboardMapping(e);
	if(e->request == MappingKeyboard)
	{
		unsigned short i;

		for(i = 0; i < arwm.X.num_screens; i++)
			grab_keys_for_screen(&arwm.X.screens[i]);
	}
}
#endif
#ifdef USE_TBAR
static void
handle_expose_event(XEvent * ev)
{
	if(ev->xexpose.count == 0)
	{
		/* xproperty was used instead of xexpose, previously.  */
		Window ev_win = ev->xexpose.window;
		Client *c = find_client(ev_win);

		if(c && (ev_win == c->info_window))
			update_info_window(c);
	}
}
#endif

static void
do_client_tidy()
{

	if(arwm.flags & ARWM_FLAG_NEED_TIDY)
	{
		Client *c;
		Client *nc;

		for(c=head_client; c; c=nc)
		{
			nc=c->next;
			if(c->flags & AR_CLIENT_REMOVE)
				remove_client(c);
		}
	}		
}


static void
arwm_process_events(void)
{
	XEvent ev;

	XNextEvent(arwm.X.dpy, &ev);
	switch (ev.type)
	{
	case EnterNotify:
		LOG("EnterNotify");
		handle_enter_event(&ev.xcrossing);
		break;
	case UnmapNotify:
		LOG("UnmapNotify");
		handle_unmap_event(&ev.xunmap);
		break;
	case PropertyNotify:
		LOG("PropertyNotify");
		handle_property_change(&ev.xproperty);
		break;
	case MapRequest:
		LOG("MapRequest");
		handle_map_request(&ev.xmaprequest);
		break;
	case KeyPress:
		LOG("KeyPress");
		arwm_handle_key_event(&ev.xkey);
		break;
#ifdef USE_TBAR
	case Expose:
		LOG("Expose");
		handle_expose_event(&ev);
		break;
#endif
	case ButtonPress:
		LOG("ButtonPress");
		arwm_handle_button_event(&ev.xbutton);
		break;
	case ConfigureRequest:
		LOG("ConfigureRequest");
		arwm_handle_configure_request(&ev.xconfigurerequest);
		break;
#ifdef USE_CMAP
	case ColormapNotify:
		LOG("ColormapNotify");
		handle_colormap_change(&ev.xcolormap);
		break;
#endif /* USE_CMAP */
#if 0
	case MappingNotify:
		LOG("MappingNotify");
		handle_mappingnotify_event(&ev.xmapping);
		break;
#endif
#ifdef USE_SHAPE
	case ShapeNotify:
		LOG("ShapeNotify");
		set_shape(find_client(ev.xany.window));
		break;
#endif /* USE_SHAPE */
	}
}

void
event_main_loop(void)
{
	/* main event loop here */
	for(;;)
	{
		arwm_process_events();
		do_client_tidy();
	}
}

