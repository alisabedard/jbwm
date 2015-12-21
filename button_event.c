/* Copyright 2008-2015, Jeffrey Bedard <jefbed@gmail.com> */

/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include "jbwm.h"

static void
move_client_with_vdesk(Client * c, Bool next)
{
	const int target_vdesk = (current->screen->vdesk + next) ? 1 : -1;

	if(next ? (target_vdesk < 10) : (target_vdesk >= 0))
	{
		switch_vdesk(current->screen, c->vdesk = target_vdesk);
		XRaiseWindow(jbwm.X.dpy, c->window);
	}
}

static void
shade(Client * c)
{
	/* This implements window shading, a substitute 
	   for iconification.  */
#ifdef EWMH
	const Atom shaded=GETATOM("_NET_WM_STATE_SHADED");
#endif//EWMH
	if(c->flags & JB_CLIENT_SHADED)
	{
		c->geometry.height=c->shade_height;
		c->flags &= ~JB_CLIENT_SHADED;
		XMapWindow(jbwm.X.dpy, c->window);
		moveresize(c);
		select_client(c);
#ifdef EWMH
		XDeleteProperty(jbwm.X.dpy, c->window, shaded);
#endif//EWMH
	}
	else
	{
		const ubyte h=TDIM+1;

		c->shade_height = c->geometry.height;
		c->ignore_unmap++;
		XUnmapWindow(jbwm.X.dpy, c->window);
		XResizeWindow(jbwm.X.dpy, c->parent, c->geometry.width, h);
		c->geometry.height = h;
		c->flags |= JB_CLIENT_SHADED;
		XSetWindowBorder(jbwm.X.dpy, c->parent, c->screen->bg.pixel);
#ifdef EWMH
		XChangeProperty(jbwm.X.dpy, c->window, shaded, XA_CARDINAL, 32,
			PropModeReplace, NULL, 0);
#endif//EWMH
	}
}

static void
button1_event(XButtonEvent * e, Client * c)
{
	const int x = e->x;
	const int y = e->y;
	const int width = c->geometry.width;

	XRaiseWindow(jbwm.X.dpy, c->parent);
	/* Text for close button press.  */
	if((x < TDIM) && (y < TDIM))
	{
		/* This fixes a bug where deleting a shaded window will cause
		   the parent window to stick around as a ghost window. */
		if(c->flags&JB_CLIENT_SHADED)
			shade(c);		
		send_wm_delete(c);
	}
	if(x > width - TDIM)
		sweep(c);	/* Resize the window.  */
	else if(x > width - TDIM - TDIM && y < TDIM)
		shade(c);
	else
		drag(c);	/* Move the window.  */
}

void
jbwm_handle_button_event(XButtonEvent * e)
{
	Client *c;

	if(!(c = find_client(e->window)))
		return;
	/* Move/Resize operations invalid on maximized windows.  */
	if(c->flags & JB_CLIENT_MAXIMIZED)
		return;
	switch (e->button)
	{
	case Button1:
		button1_event(e, c);
		break;
	case Button2:
		XLowerWindow(jbwm.X.dpy, c->parent);
		break;
	case Button3:
		/* Resize operations more useful here,
		   rather than for third button, for laptop
		   users especially, where it is difficult
		   to register a middle button press, even
		   with X Emulate3Buttons enabled.  */
		sweep(c);
		break;
	case Button4:
		move_client_with_vdesk(c, true);
		break;
	case Button5:
		move_client_with_vdesk(c, false);
		break;
	}
}
