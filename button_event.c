/* Copyright 2008, Jeffrey Bedard <antiright@gmail.com> */

/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include "jbwm.h"

static void
move_client_with_vdesk(Client * c, Bool next)
{
	ScreenInfo *current_screen = current->screen;
	const int target_vdesk =
		current_screen->vdesk + next ? 1 : -1;

	if(next ? (target_vdesk < 10) : (target_vdesk >= 0))
	{
		switch_vdesk(current_screen, c->vdesk = target_vdesk);
		XRaiseWindow(jbwm.X.dpy, c->window);
	}
}

#ifdef USE_SHADE
static void
shade_window(Client * c)
{
	/* This implements window shading, a substitute for iconification.  */
	if(c->geometry.height == TITLEBAR_HEIGHT)
	{
		/* Unshade.  */
		c->geometry.height = c->shade_height;
		c->flags &= ~AR_CLIENT_SHADED;
		moveresize(c);
	}
	else
	{
		c->shade_height = c->geometry.height;
		XResizeWindow(jbwm.X.dpy, c->parent,
			c->geometry.width, TITLEBAR_HEIGHT);
		c->geometry.height = TITLEBAR_HEIGHT;
		c->flags |= AR_CLIENT_SHADED;
	}
}
#endif /* USE_SHADE */

static void
button1_event(XButtonEvent * e, Client * c)
{
	const int x = e->x;
#ifdef USE_CLOSE_BUTTON
	const int y = e->y;
#endif
	const int width = c->geometry.width;

	XRaiseWindow(jbwm.X.dpy, c->parent);
	/* Text for close button press.  */
#ifdef USE_CLOSE_BUTTON
	if((x < AR_BUTTON_WIDTH) && (y < TITLEBAR_HEIGHT))
		send_wm_delete(c, !(e->state & jbwm.keymasks.mod));
#endif
	if(x > width - AR_RESIZE_DELTA)
		sweep(c);	/* Resize the window.  */
#ifdef USE_SHADE
	else if(x > width - AR_SHADE_DELTA && y < TITLEBAR_HEIGHT)
		shade_window(c);
#endif /* USE_SHADE */
	else
		drag(c);	/* Move the window.  */
}

void
jbwm_handle_button_event(XButtonEvent * e)
{
	Client *c;

	if(!(c = find_client(e->window)))
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
		drag(c);
		break;
	case Button4:
		move_client_with_vdesk(c, True);
		break;
	case Button5:
		move_client_with_vdesk(c, False);
		break;
	}
}
