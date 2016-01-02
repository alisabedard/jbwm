/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include <stdlib.h>
#include "jbwm.h"

static ScreenInfo *
find_screen(Window root)
{
	for(ubyte i = 0; i < jbwm.X.num_screens; i++)
		if(jbwm.X.screens[i].root == root)
			return &jbwm.X.screens[i];
	return NULL;
}

static void
cleanup()
{
	LOG("cleanup()");
	jbwm.need_cleanup=0;
	Client *i;
	for(Client *c = jbwm.head; c; c = i)
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
	Client *c=find_client(e->window);
	if(!c) return;
	LOG("handle_unmap_event(e): %d ignores remaining", c->ignore_unmap);
	if(c->ignore_unmap<1)
	{
		LOG("!c->ignore_unmap");
		c->flags |= JB_CLIENT_REMOVE;
		jbwm.need_cleanup=true;
	}
	else c->ignore_unmap--;
}

#ifdef USE_CMAP
static void
handle_colormap_change(XColormapEvent * e)
{
	LOG("handle_colormap_change(e)");
	Client *c = find_client(e->window);
	if(c && e->new)
	{
		c->cmap = e->colormap;
		XInstallColormap(D, c->cmap);
	}
}
#endif /* USE_CMAP */

static void
handle_wm_hints(Client *c)
{
	XWMHints *h=XGetWMHints(D, c->window);
	if(h->flags & XUrgencyHint)
	{
		switch_vdesk(c->screen, c->screen->vdesk);
		unhide(c);
		XRaiseWindow(D, c->parent);
	}
	XFree(h);
}

#ifdef DEBUG
void
print_atom(const Atom a, const unsigned int line)
{
        char *an=XGetAtomName(D, a);
        fprintf(stderr, "\t%s:%d %s(%lu)\n", __FILE__, line, an, a);
        XFree(an);
}       
#endif//DEBUG

static void
handle_property_change(XPropertyEvent * e)
{
	Client *c=find_client(e->window);
	if(!c) return;
	const Atom a=e->atom;
	if(a==XA_WM_HINTS) handle_wm_hints(c);
#ifdef USE_TBAR
	else if(a==XA_WM_NAME) update_titlebar(c);
	else moveresize(c); // Required to show titlebar on new client
#endif//USE_TBAR
}

static void
handle_enter_event(XCrossingEvent * e)
{
	Client *c=find_client(e->window);
	if(c) select_client(c);
}

#ifdef USE_TBAR
static void
handle_expose_event(XEvent * ev)
{
	// Ignore extra expose events
	if(ev->xexpose.count == 0)
	{
		/* xproperty was used instead of xexpose, previously.  */
		const Window w = ev->xexpose.window;
		Client *c=find_client(w);
		if(!c) return;
		if(w==c->titlebar) update_titlebar(c);
	}
}
#endif//USE_TBAR

static void
jbwm_handle_configure_request(XConfigureRequestEvent * e)
{
	XWindowChanges wc={.x=e->x, .y=e->y, .width=e->width, .height=e->height,
		.border_width=e->border_width, .sibling=e->above, 
		.stack_mode=e->detail};
	XConfigureWindow(D, e->window, e->value_mask, &wc);
}

void
main_event_loop(void)
{
	XEvent ev;
head:
	XNextEvent(D, &ev);
	switch (ev.type)
	{
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
		if(!find_client(ev.xmaprequest.window))
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
		jbwm_handle_configure_request(&ev.xconfigurerequest);
		break;
	case ConfigureNotify:
		if(!ev.xconfigure.override_redirect)
		{
			Client *c=find_client(ev.xconfigure.window);
			if(c) moveresize(c);
		}
		break;
#ifdef EWMH
	case CreateNotify:
	case DestroyNotify:
		ewmh_update_client_list();
		break;
#endif//EWMH
#ifdef USE_CMAP
	case ColormapNotify:
		handle_colormap_change(&ev.xcolormap);
		break;
#endif /* USE_CMAP */
#ifdef EWMH
	case ClientMessage:
		ewmh_client_message(&ev.xclient);
		break;
#endif//EWMH
#ifdef USE_SHAPE
	case ShapeNotify:
		LOG("ShapeNotify");
		set_shape(find_client(ev.xany.window));
		break;
#endif /* USE_SHAPE */
#ifdef DEBUG
	case MapNotify:
	case KeyRelease:
	case MappingNotify:
	case ReparentNotify:
		break;
	default:
		LOG("Unhandled event (%d)", ev.type);
#endif//DEBUG
	}
	if(jbwm.need_cleanup)
		cleanup();
	goto head;
}

