/* Copyright 2008-2015, Jeffrey Bedard <jefbed@gmail.com> */

/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include "jbwm.h"

#ifdef USE_TBAR
void
shade(Client * c)
{
	/* This implements window shading, a substitute 
	   for iconification.  */
	if(c->flags & JB_CLIENT_SHADED)
	{
		c->geometry.height=c->shade_height;
		c->flags &= ~JB_CLIENT_SHADED;
		XMapWindow(D, c->window);
		moveresize(c);
		select_client(c);
#ifdef EWMH
		XDeleteProperty(D, c->window, ewmh.WM_STATE_SHADED);
#endif//EWMH
	}
	else
	{
		c->shade_height = c->geometry.height;
		c->ignore_unmap++;
		XUnmapWindow(D, c->window);
		const ubyte h=c->geometry.height=TDIM+1;
		XResizeWindow(D, c->parent, c->geometry.width, h);
		c->flags |= JB_CLIENT_SHADED;
		XSetWindowBorder(D, c->parent, c->screen->bg.pixel);
#ifdef EWMH
		XPROP(c->window, ewmh.WM_STATE, XA_CARDINAL, 
			&ewmh.WM_STATE_SHADED, 1);
#endif//EWMH
	}
}
#endif//USE_TBAR

static void
button1_event(XButtonEvent * e, Client * c)
{
	const int width = c->geometry.width;

	XRaiseWindow(D, c->parent);
#ifdef USE_TBAR
	const Position p={.x=e->x,.y=e->y};
	if((p.x < TDIM) && (p.y < TDIM)) // Close button
	{
		/* This fixes a bug where deleting a shaded window will cause
		   the parent window to stick around as a ghost window. */
		if(c->flags&JB_CLIENT_SHADED)
			shade(c);		
		send_wm_delete(c);
	}
	if(p.x > width - TDIM) // Resize button
		sweep(c);	
	else if(p.x > width - TDIM - TDIM && p.y < TDIM) // Shade button
		shade(c);
	else drag(c);	// Handle
#else//!USE_TBAR
	drag(c); // Move the window
#endif//USE_TBAR
}

void
jbwm_handle_button_event(XButtonEvent * e)
{
	Client *c=find_client(e->window);
	// Return if invalid event.  
	if(!c) return;
	/* Move/Resize operations invalid on maximized windows.  */
	if(c->flags & JB_CLIENT_MAXIMIZED) return;
	switch (e->button)
	{
	case Button1:
		button1_event(e, c);
		break;
	case Button2:
		XLowerWindow(D, c->parent);
		break;
	case Button3:
		/* Resize operations more useful here,
		   rather than for third button, for laptop
		   users especially, where it is difficult
		   to register a middle button press, even
		   with X Emulate3Buttons enabled.  */
		sweep(c);
		break;
	}
}
