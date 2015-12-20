/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include "jbwm.h"

void
configure(Client *c)
{
	XConfigureEvent *e;
	XRectangle *g;

	e=&(c->ce);
	e->event=c->window;
	g=&(c->geometry);
	e->x=g->x;
	e->y=g->y;
	e->width=g->width;
	e->height=g->height;
}

void
send_configure(Client *c)
{
	configure(c);
	XSendEvent(jbwm.X.dpy, c->window, false, StructureNotifyMask,
		(XEvent *)&(c->ce));
}

/*
 * used all over the place.  return the client that has specified window as
 * either window or parent
 */
Client *
find_client(Window w)
{
	Client *c;

	for(c=head_client; c; c=c->next)
	{
		if(w==c->parent || w==c->window || w==c->titlebar)
		{
			return c;
		}
	}

	return NULL;
#if 0
	for(c = head_client; c && (w != c->parent) && (w != c->window)
#ifdef USE_TBAR /* Allow titlebar to receive events: */
		&& (w != c->titlebar)
#endif
		; c = c->next)
		/* empty */ ;

	/* non-null if client found.  */
	return c;
#endif
}

void
set_wm_state(Client * c, int state)
{
	/*
	 * Using "long" for the type of "data" looks wrong, but the fine
	 * people in the X Consortium defined it this way (even on 64-bit
	 * machines).
	 */
	long data[2];		/* = { state, None }; */
	const Atom sa=GETATOM("WM_STATE");

	data[0] = state;
	XChangeProperty(jbwm.X.dpy, c->window, sa, sa, 32, 
		PropModeReplace, (unsigned char *)data, 2);
}

void
initialize_client_ce(Client * c)
{
	c->ce.type = ConfigureNotify;
	c->ce.border_width = 0;
	c->ce.above = None;
	c->ce.override_redirect = false;
	c->ce.window = c->window;
}

void
select_client(Client * c)
{
	if(current)
	{
		XSetWindowBorder(jbwm.X.dpy, current->parent,
			current->screen->bg.pixel);
		current->flags &= ~JB_CLIENT_ACTIVE;
	}
	if(c)
	{
		c->flags |= JB_CLIENT_ACTIVE;
		XSetWindowBorder(jbwm.X.dpy, c->parent,
			!is_sticky(c) ? c->screen->fg.pixel : c->
			screen->fc.pixel);

#ifdef USE_CMAP
		XInstallColormap(jbwm.X.dpy, c->cmap);
#endif /* USE_CMAP */
		XSetInputFocus(jbwm.X.dpy, c->window,
			RevertToPointerRoot, CurrentTime);
	}
	current = c;
}

void
fix_client(Client * c)
{
#if 0
	if(c->vdesk != c->screen->vdesk)
		return;
#endif
	c->vdesk=c->screen->vdesk;
	toggle_sticky(c);
	select_client(c);
}

static inline void
relink_window_list(Client * c)
{
	LOG("relink_window_list()");
	if(head_client == c)
	{
		head_client = c->next;
	}
	else
	{
		Client *p;

		for(p = head_client; p && p->next; p = p->next)
		{
			if(p->next == c)
			{
				p->next = c->next;
				return;
			}
		}
	}
}

static void
unparent_window(Client * c)
{
	const Window w=c->window;
	const Window p=c->parent;

	LOG("unparent_window()");
	if(!p)
	{
		LOG("Already unparented!");
		return;
	}
	XReparentWindow(jbwm.X.dpy, w, c->screen->root, c->geometry.x, 
		c->geometry.y);
	XRemoveFromSaveSet(jbwm.X.dpy, w);
	if(p)
		XDestroyWindow(jbwm.X.dpy, p);
	c->parent=0;
}

void
remove_client(Client * c)
{
	Display *d=jbwm.X.dpy;
	const Window w=c->window;

	LOG("remove_client()");
	XGrabServer(d);
	if(c->flags & JB_CLIENT_REMOVE)
	{
		LOG("JB_CLIENT_REMOVE");
		set_wm_state(c, WithdrawnState);
		XDeleteProperty(d, w, JA_VWM_DESKTOP);
		XDeleteProperty(d, w, JA_VWM_STATE);
	}
#ifdef USE_TBAR
	delete_titlebar(c);
#endif /* USE_TBAR */
	unparent_window(c);
	relink_window_list(c);
	if(current == c)
		current = NULL;
	free(c);
	XUngrabServer(d);
	XSync(d, false);
}

void
xmsg(Window w, Atom a, long x)
{
	XEvent ev;

	ev.type = ClientMessage;
	ev.xclient.window = w;
	ev.xclient.message_type = a;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = x;
	ev.xclient.data.l[1] = CurrentTime;

	LOG("xmsg()");
	XSendEvent(jbwm.X.dpy, w, false, NoEventMask, &ev);
}

void
send_wm_delete(Client * c)
{
	LOG("send_wm_delete()");
	xmsg(c->window, GETATOM("WM_PROTOCOLS"),
		GETATOM("WM_DELETE_WINDOW"));
}

#ifdef USE_SHAPE
bool
is_shaped(Client * c)
{
	int bounding_shaped;
	int i;
	unsigned int u;
	return XShapeQueryExtents(jbwm.X.dpy, c->window, &bounding_shaped,
		&i, &i, &u, &u, &i, &i, &i, &u, &u)
		&& bounding_shaped;
}

void
set_shape(Client * c)
{
	/* Validate inputs:  Make sure that the SHAPE extension is
	   available, and make sure that C is initialized.  */
	if(!jbwm.X.have_shape || !c)
		return;
	/*
	 * Logic to decide if we have a shaped window cribbed from
	 * fvwm-2.5.10. Previous method (more than one rectangle returned
	 * from XShapeGetRectangles) worked _most_ of the time.
	 */
	if(is_shaped(c))
	{
		XShapeCombineShape(jbwm.X.dpy, c->parent,
			ShapeBounding, 0, TDIM, c->window, 
			ShapeBounding, ShapeSet);
	}
}
#endif
