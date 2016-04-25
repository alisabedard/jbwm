// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <stdint.h>
#include <X11/Xlib.h>
#include "client.h"
#include "client_t.h"
#include "jbwmenv.h"
#include "screen.h"
#include "titlebar.h"

static void button1_event(XButtonEvent * e
#ifndef USE_TBAR
			  __attribute__ ((unused))
#endif//!USE_TBAR
			  , Client * c)
{
	XRaiseWindow(jbwm.dpy, c->parent);
#ifdef USE_TBAR
	const uint16_t w = c->size.width;
	const XPoint p = { e->x, e->y };
	const uint32_t f = c->flags;
	if (!(f & JB_NO_CLOSE) && (p.x < TDIM)
	    && (p.y < TDIM)) {	// Close button
		/* This fixes a bug where deleting a shaded window will cause
		   the parent window to stick around as a ghost window. */
		if (f & JB_SHADED) 
			shade(c);

		send_wm_delete(c);
	} else if(!(f & JB_NO_RESIZE) && (p.x > w - TDIM)) { // Resize button
		resize(c);
	}
	else if (!(f & JB_NO_MIN) && (p.x > w - TDIM - TDIM && p.y < TDIM)) {
		shade(c);
	}
	else			// Handle
#endif//USE_TBAR
		drag(c);	// Move the window
}

void jbwm_handle_button_event(XButtonEvent * e)
{
	Client *c = find_client(e->window);

	// Return if invalid event or maximized
	if (!c || c->flags & JB_MAXIMIZED)
		return;

	switch (e->button) {
	case Button1:
		button1_event(e, c);
		break;

	case Button2:
		XLowerWindow(jbwm.dpy, c->parent);
		break;

	case Button3:
		/* Resize operations more useful here,
		   rather than for third button, for laptop
		   users especially, where it is difficult
		   to register a middle button press, even
		   with X Emulate3Buttons enabled.  */
		c->flags & JB_SHADED ? drag(c) : resize(c);
		break;
	}
}
