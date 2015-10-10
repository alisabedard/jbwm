/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include <stdlib.h>
#include "jbwm.h"

void
jbwm_current_to_head(void)
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

	for(i = 0; i < jbwm.X.num_screens; i++)
		if(jbwm.X.screens[i].root == root)
			return &jbwm.X.screens[i];

	return NULL;
}

static void
handle_map_request(XMapRequestEvent * e)
{
	Client *c = find_client(e->window);

	if(c)
	{
		/* Avoid mapping any ghost windows.  */
		if(c->flags & JB_CLIENT_REMOVE)
			return;
		if(!(c->flags & JB_CLIENT_IS_STICKY) && 
			(c->vdesk != c->screen->vdesk))
		{
			switch_vdesk(c->screen, c->vdesk);
		}
		unhide(c);
		c->ignore_unmap++;
	}
	else
	{
		XWindowAttributes attr;

		XGetWindowAttributes(jbwm.X.dpy, e->window, &attr);
		make_new_client(e->window, find_screen(attr.root));
	}
}

static void
cleanup()
{
	Client *c, *i;

	jbwm.need_cleanup=0;
	for(c = head_client; c; c = i)
	{
		i = c->next;
		if(c->flags & JB_CLIENT_REMOVE)
			remove_client(c);
		if(!i)
			return;
	}
}

static void
handle_unmap_event(XUnmapEvent * e)
{
	Client *c;

	if((c = find_client(e->window)))
	{
		if(c->ignore_unmap)
		{
			c->ignore_unmap--;
		}
		else
		{
			c->flags |= JB_CLIENT_REMOVE;
			jbwm.need_cleanup=1;
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
		XInstallColormap(jbwm.X.dpy, c->cmap);
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
		jbwm_current_to_head();
	}
}

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

void
main_event_loop(void)
{
	XEvent ev;
head:
	XNextEvent(jbwm.X.dpy, &ev);
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
		jbwm_handle_key_event(&ev.xkey);
		break;
#ifdef USE_TBAR
	case Expose:
		LOG("Expose");
		handle_expose_event(&ev);
		break;
#endif
	case ButtonPress:
		LOG("ButtonPress");
		jbwm_handle_button_event(&ev.xbutton);
		break;
	case ConfigureRequest:
		LOG("ConfigureRequest");
		jbwm_handle_configure_request(&ev.xconfigurerequest);
		break;
#ifdef USE_CMAP
	case ColormapNotify:
		LOG("ColormapNotify");
		handle_colormap_change(&ev.xcolormap);
		break;
#endif /* USE_CMAP */
#ifdef USE_SHAPE
	case ShapeNotify:
		LOG("ShapeNotify");
		set_shape(find_client(ev.xany.window));
		break;
#endif /* USE_SHAPE */
	}
	if(jbwm.need_cleanup)
		cleanup();
	goto head;
}

