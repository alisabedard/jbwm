/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include "jbwm.h"

/*
 * used all over the place.  return the client that has specified window as
 * either window or parent
 */

Client *
find_client(Window w)
{
	Client *c;

	for(c = head_client; c && (w != c->parent) && (w != c->window)
		/* This allows the info_window member, which is the 
		 * titlebar parent, to match events.  Particularaly, 
		 * it allows the titlebar to be used as a drag handle.  */
#ifdef USE_TBAR
		&& (w != c->info_window)
#endif
		; c = c->next)
		/* empty */ ;

	/* non-null if client found.  */
	return c;
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

	const Atom state_atom = JBWM_ATOM_WM_STATE;

	data[0] = state;
	XChangeProperty(jbwm.X.dpy, c->window, state_atom, state_atom,
		32, PropModeReplace, (unsigned char *)data, 2);
}

void
initialize_client_ce(Client * c)
{
	c->ce.type = ConfigureNotify;
	c->ce.border_width = 0;
	c->ce.above = None;
	c->ce.override_redirect = False;
	c->ce.window = c->window;
}

void
send_config(Client * c)
{
	SET_CLIENT_CE(c);
	SEND_CONFIG(c);
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
	if(c->vdesk != c->screen->vdesk)
		return;
	toggle_sticky(c);
	select_client(c);
}

static void
relink_window_list(Client * c)
{
	if(head_client == c)
		head_client = c->next;
	else
	{
		Client *p;

		for(p = head_client; p && p->next; p = p->next)
			if(p->next == c)
				p->next = c->next;
	}
}

static void
unparent_window(Client * c)
{
	XReparentWindow(jbwm.X.dpy, c->window, c->screen->root,
		c->geometry.x, c->geometry.y);
	XRemoveFromSaveSet(jbwm.X.dpy, c->window);
	XDestroyWindow(jbwm.X.dpy, c->parent);
}

void
remove_client(Client * c)
{
	XGrabServer(jbwm.X.dpy);
#ifdef USE_TBAR
	remove_info_window(c);
#endif /* USE_TBAR */
	unparent_window(c);
	relink_window_list(c);
	if(current == c)
		current = NULL;
	if(c->size)
		XFree(c->size);
	free(c);
	XUngrabServer(jbwm.X.dpy);
	XSync(jbwm.X.dpy, False);
}

static int
send_xmessage(Window w, Atom a, long x)
{
	XEvent ev;

	ev.type = ClientMessage;
	ev.xclient.window = w;
	ev.xclient.message_type = a;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = x;
	ev.xclient.data.l[1] = CurrentTime;

	return XSendEvent(jbwm.X.dpy, w, False, NoEventMask, &ev);
}

void
send_wm_delete(Client * c, int kill_client)
{
	XUnmapWindow(jbwm.X.dpy, c->parent);
	if(kill_client)
		send_xmessage(c->window, JBWM_ATOM_WM_PROTOS,
			JBWM_ATOM_WM_DELETE);
	else
		XKillClient(jbwm.X.dpy, c->window);
}

#ifdef USE_SHAPE
void
set_shape(Client * c)
{
	int bounding_shaped;
	int i, b;
	unsigned int u;		/* dummies */

	/* Validate inputs:  Make sure that the SHAPE extension is
	   available, and make sure that C is initialized.  */
	if(!jbwm.X.have_shape || !c)
		return;
	/*
	 * Logic to decide if we have a shaped window cribbed from
	 * fvwm-2.5.10. Previous method (more than one rectangle returned
	 * from XShapeGetRectangles) worked _most_ of the time.
	 */
	XShapeSelectInput(jbwm.X.dpy, c->window, ShapeNotifyMask);
	if(XShapeQueryExtents(jbwm.X.dpy, c->window, &bounding_shaped,
		&i, &i, &u, &u, &b, &i, &i, &u, &u)
		&& bounding_shaped)
	{
		c->flags |=
			JB_CLIENT_SHAPED |
			JB_CLIENT_DONT_USE_TITLEBAR;
		XShapeCombineShape(jbwm.X.dpy, c->parent,
			ShapeBounding, 0, 0, c->window, ShapeBounding,
			ShapeSet);
	}
}
#endif
