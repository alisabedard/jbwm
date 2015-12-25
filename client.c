/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include "jbwm.h"

void
configure(Client *c)
{
	XConfigureEvent *e;
	e=&(c->ce);
	e->event=c->window;
	XRectangle *g=&(c->geometry);
	e->x=g->x;
	e->y=g->y;
	e->width=g->width;
	e->height=g->height;
}

void
send_configure(Client *c)
{
	configure(c);
	XSendEvent(D, c->window, false, StructureNotifyMask,
		(XEvent *)&(c->ce));
}

/*
 * used all over the place.  return the client that has specified window as
 * either window or parent
 */
Client *
find_client(Window w)
{
	for(Client *c=head_client; c; c=c->next)
		if(w==c->parent || w==c->window 
#ifdef USE_TBAR
			|| w==c->titlebar
#endif//USE_TBAR
			) return c;
	return NULL;
}

void
set_wm_state(Client * c, int state)
{
	unsigned char data[2];		// = { state, None }; 

	data[0] = state;
	XPROP(c->window, XA("WM_STATE"), XA_CARDINAL, data, 2);
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
		XSetWindowBorder(D, current->parent,
			current->screen->bg.pixel);
		current->flags &= ~JB_CLIENT_ACTIVE;
	}
	if(c)
	{
		c->flags |= JB_CLIENT_ACTIVE;
		XSetWindowBorder(D, c->parent, !is_sticky(c) 
			? c->screen->fg.pixel : c->screen->fc.pixel);
#ifdef USE_CMAP
		XInstallColormap(D, c->cmap);
#endif /* USE_CMAP */
		XSetInputFocus(D, c->window, RevertToPointerRoot, CurrentTime);
#ifdef EWMH
		XPROP(c->screen->root, ewmh.ACTIVE_WINDOW, XA_WINDOW,
			&(c->window), 1);
#endif//EWMH
	}
	current = c;
}

void
fix_client(Client * c)
{
	c->vdesk=c->screen->vdesk;
	toggle_sticky(c);
	select_client(c);
}

static inline void
relink_window_list(Client * c)
{
	LOG("relink_window_list()");
	if(head_client == c) head_client = c->next;
	else for(Client *p = head_client; p && p->next; p = p->next)
		if(p->next == c)
		{
			p->next = c->next;
			return;
		}
}

static void
unparent_window(Client * c)
{
	LOG("unparent_window()");
	if(!c->parent) return;
	XReparentWindow(D, c->window, c->screen->root, c->geometry.x, 
		c->geometry.y);
	XRemoveFromSaveSet(D, c->window);
	if(c->parent) XDestroyWindow(D, c->parent);
	c->parent=0;
}

void
remove_client(Client * c)
{
	if(c->flags&JB_CLIENT_REMOVE)
	{
#ifdef EWMH
		XDeleteProperty(D, c->window, ewmh.WM_DESKTOP);
		XDeleteProperty(D, c->window, ewmh.WM_STATE);
#endif//EWMH
		set_wm_state(c, WithdrawnState);
	}
#ifdef EWMH
	else XDeleteProperty(D, c->window, ewmh.WM_ALLOWED_ACTIONS);
#endif//EWMH
	unparent_window(c);
	relink_window_list(c);
	if(current==c) current=NULL;
	free(c);
}

void
xmsg(Window w, Atom a, long x)
{
	XEvent ev = {.xclient.type=ClientMessage, .xclient.window=w, 
		.xclient.message_type=a, .xclient.format=32, 
		.xclient.data.l[0]=x, .xclient.data.l[1]=CurrentTime};
	XSendEvent(D, w, false, NoEventMask, &ev);
}

void
send_wm_delete(Client * c)
{
	xmsg(c->window, XA("WM_PROTOCOLS"), XA("WM_DELETE_WINDOW"));
}

#ifdef USE_SHAPE
bool
is_shaped(Client * c)
{
	int bounding_shaped;
	int i;
	unsigned int u;
	return XShapeQueryExtents(D, c->window, &bounding_shaped,
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
		XShapeCombineShape(D, c->parent,
			ShapeBounding, 0, TDIM, c->window, 
			ShapeBounding, ShapeSet);
	}
}
#endif//USE_SHAPE
