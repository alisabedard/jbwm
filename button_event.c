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
		c->size.height=c->shade_height;
		c->flags &= ~JB_CLIENT_SHADED;
		XMapWindow(D, c->window);
		moveresize(c);
#if 0 // FIXME: should be deleted from WM_STATE list
#ifdef EWMH
		XDeleteProperty(D, c->window, ewmh.WM_STATE_SHADED);
#endif//EWMH
#endif//0
	}
	else // Shade the client
	{
		c->shade_height = c->size.height;
		c->ignore_unmap++;
		XUnmapWindow(D, c->window);
		const ubyte h=c->size.height=TDIM+1;
		XResizeWindow(D, c->parent, c->size.width, h);
		c->flags |= JB_CLIENT_SHADED;
#if 0 // FIXME: Should be addded to WM_STATE list
#ifdef EWMH
		XPROP(c->window, ewmh.WM_STATE, XA_CARDINAL, 
			&ewmh.WM_STATE_SHADED, 1);
#endif//EWMH
#endif//0
		select_client(c);
	}
}
#endif//USE_TBAR

static void
button1_event(XButtonEvent * e, Client * c)
{
	const int width = c->size.width;

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
