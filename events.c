/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include <stdlib.h>
#include "jbwm.h"

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

	LOG("handle_map_request(e)");
	if(c)
	{
		/* Avoid mapping any ghost windows.  */
		if(c->flags & JB_CLIENT_REMOVE)
			return;
		unhide(c);
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

	LOG("cleanup()");
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

	LOG("handle_unmap_event(e)");
	LOG("send_event: %s", e->send_event?"true":"false");
	LOG("from_configure: %s", e->from_configure?"true":"false");
		
	if((c = find_client(e->window)))
	{
		LOG("%d ignores remaining", c->ignore_unmap);
		if(c->ignore_unmap<1)
		{
			LOG("!c->ignore_unmap");
			c->flags |= JB_CLIENT_REMOVE;
			jbwm.need_cleanup=1;
		}
		else
		{
			c->ignore_unmap--;
		}
	}
}

#ifdef USE_CMAP
static void
handle_colormap_change(XColormapEvent * e)
{
	Client *c = find_client(e->window);
	
	LOG("handle_colormap_change(e)");
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
	const Atom a = e->atom;

	LOG("handle_property_change(e)");

	c = find_client(e->window);
	if(c)
	{
		moveresize(c);
		switch(a)
		{
		case XA_WM_NORMAL_HINTS:
			LOG("XA_WM_NORMAL_HINTS");
			get_wm_normal_hints(c);
			break;
#ifdef USE_TBAR
		case XA_WM_NAME:
			LOG("XA_WM_NAME");
			update_info_window(c);
			break;
#endif /* USE_TBAR */
#ifdef DEBUG
		default:
			{
				char *n;

				n=XGetAtomName(jbwm.X.dpy, a);
				LOG("Atom %u: %s", (unsigned int)a, n);
				XFree(n);
			}
#endif /* DEBUG */
		}
	}
}

static void
handle_enter_event(XCrossingEvent * e)
{
	Client *c;

	if((c = find_client(e->window)))
	{
		/* Make sure event is on current desktop and only process
			event on the application window.  */
		if(is_sticky(c))
			goto skip;
		if((c->vdesk != c->screen->vdesk) || (e->window != c->window))
			return;
skip:
		select_client(c);
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

static void
jbwm_handle_configure_request(XConfigureRequestEvent * e)
{
	Client *c;
	XWindowChanges wc;

	c=find_client(e->window);
	wc.x=e->x;
	wc.y=e->y;
	wc.width=e->width;
	wc.height=e->height;
	wc.border_width=0;
	wc.sibling=e->above;
	wc.stack_mode=e->detail;
	if(c)
	{
		if(e->value_mask & CWStackMode && e->value_mask & CWSibling)
		{
			Client *s;

			s = find_client(e->above);
			if(s)
				wc.sibling=s->parent;
		}
	}
	else
		XConfigureWindow(jbwm.X.dpy, e->window, e->value_mask, &wc);
}

void
main_event_loop(void)
{
	XEvent ev;
head:
	XNextEvent(jbwm.X.dpy, &ev);
	switch (ev.type)
	{
	case EnterNotify:
		handle_enter_event(&ev.xcrossing);
		break;
	case UnmapNotify:
		LOG("UnmapNotify");
		handle_unmap_event(&ev.xunmap);
		break;
	case ClientMessage:
		LOG("ClientMessage");
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

