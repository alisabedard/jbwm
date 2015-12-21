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
	for(Client *c = head_client; c; c = i)
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
	LOG("handle_unmap_event(e)");
	LOG("send_event: %s", e->send_event?"true":"false");
	LOG("from_configure: %s", e->from_configure?"true":"false");
	Client *c=find_client(e->window);
	if(!c) return;
	LOG("%d ignores remaining", c->ignore_unmap);
	if(c->ignore_unmap<1)
	{
		LOG("!c->ignore_unmap");
		c->flags |= JB_CLIENT_REMOVE;
		jbwm.need_cleanup=1;
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
		XInstallColormap(jbwm.X.dpy, c->cmap);
	}
}
#endif /* USE_CMAP */

static void
handle_wm_hints(Client *c)
{
	XWMHints *h=XGetWMHints(jbwm.X.dpy, c->window);
	if(h->flags & XUrgencyHint)
	{
		switch_vdesk(c->screen, c->screen->vdesk);
		unhide(c);
		XRaiseWindow(jbwm.X.dpy, c->parent);
	}
	XFree(h);
}

static void
handle_property_change(XPropertyEvent * e)
{
	Client *c=find_client(e->window);
	if(!c) return;
#ifdef DEBUG
	char *an=XGetAtomName(jbwm.X.dpy, e->atom);
	LOG("atom: %s(%lu)", an, e->atom);
	XFree(an);
#endif//DEBUG
	switch(e->atom)
	{ 
	case XA_WM_NAME:
		update_titlebar(c);
		break;
	case XA_WM_HINTS:
		handle_wm_hints(c);
		break;
	default:
		moveresize(c);
	}
}

static void
handle_enter_event(XCrossingEvent * e)
{
	Client *c=find_client(e->window);
	if(!c) return;
	/* Make sure event is on current desktop and only process event 
	   on the application window.  */
	if(is_sticky(c)) goto skip;
	if((c->vdesk != c->screen->vdesk) || (e->window != c->window))
		return;
skip:
		select_client(c);
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
	XConfigureWindow(jbwm.X.dpy, e->window, e->value_mask, &wc);
}

static void
handle_client_message(XClientMessageEvent *e)
{
	// Define the atoms we'll use:
	struct Messages { const Atom desktop, active, close, state, fullscreen;
		} m = { .desktop=GETATOM("_NET_WM_DESKTOP"), 
		.active=GETATOM("_NET_ACTIVE_WINDOW"), 
		.close=GETATOM("_NET_CLOSE_WINDOW"),
		.state=GETATOM("_NET_WM_STATE"), 
		.fullscreen=GETATOM("_NET_WM_STATE_FULLSCREEN")};
	Client *c=find_client(e->window);
	if(!c) return;
	const Atom t=e->message_type;
	if(t==m.desktop) switch_vdesk(c->screen, e->data.l[0]);
	else if(t==m.active) select_client(c);
	else if(t==m.close && e->data.l[1]==2) send_wm_delete(c);
	else if(t==m.state)
	{
		bool m=false;
		for(int i=1; i<=2; i++) if((Atom)e->data.l[i] 
			== GETATOM("_NET_WM_STATE_FULLSCREEN")) m=true;
		if(m) maximize(c);
	} 
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
#ifdef USE_CMAP
	case ColormapNotify:
		handle_colormap_change(&ev.xcolormap);
		break;
#endif /* USE_CMAP */
	case ClientMessage:
		handle_client_message(&ev.xclient);
		break;
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

