/* Copyright 2008-2016, Jeffrey Bedard <jefbed@gmail.com> */

/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include "jbwm.h"

static void
button1_event(XButtonEvent * e
#ifndef USE_TBAR
	__attribute__((unused))
#endif//!USE_TBAR
	, Client * c)
{
	XRaiseWindow(D, c->parent);
#ifdef USE_TBAR
	const Position p={.x=e->x,.y=e->y};
	const int width = c->size.width;
	if((p.x < TDIM) && (p.y < TDIM)) // Close button
	{
		// Honor !MWM_FUNC_CLOSE
		if(c->flags & JB_NO_CLOSE)
			goto drag;
		/* This fixes a bug where deleting a shaded window will cause
		   the parent window to stick around as a ghost window. */
		if(c->flags&JB_SHADED)
			shade(c);		
		send_wm_delete(c);
	}
	if(p.x > width - TDIM) // Resize button
	{
		if(c->flags & JB_NO_RESIZE)
			goto drag;
		sweep(c);	
	}
	else if(p.x > width - TDIM - TDIM && p.y < TDIM) // Shade button
	{
		if(c->flags & JB_NO_MIN)
			goto drag;
		shade(c);
	}
	else // Handle
drag:
#endif//USE_TBAR
		drag(c); // Move the window
}

void
jbwm_handle_button_event(XButtonEvent * e)
{
	Client *c=find_client(e->window);
	// Return if invalid event.  
	if(!c) return;
	/* Move/Resize operations invalid on maximized windows.  */
	if(c->flags & JB_MAXIMIZED) return;
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
		if(c->flags&JB_SHADED)
			drag(c);
		else
			sweep(c);
		break;
	}
}
