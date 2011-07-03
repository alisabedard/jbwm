/* Copyright 2008, Jeffrey Bedard <antiright@gmail.com> */

/* arwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <arwm@6809.org.uk>
 * see README for license and other details. */


#include "arwm.h"

#ifdef SCROLL_WHEEL_PAGER
static void
move_client_with_vdesk(Client * c, Bool next)
{
	ScreenInfo * current_screen=find_current_screen();
	const int target_vdesk=current_screen->vdesk 
		+ next ? 1 : -1;

	if(next ? (target_vdesk < 8) : (target_vdesk>=0))
	{
		switch_vdesk(current_screen, 
			c->vdesk=target_vdesk);
		XRaiseWindow(arwm.X.dpy, c->window);
	}
}
#endif

void 
arwm_handle_button_event(XButtonEvent *e) 
{
	Client *c = find_client(e->window);

	if (c) switch (e->button) 
	{
	case Button1:
	{
		const int x=e->x;
#ifdef TITLEBAR
		const int y=e->y;
		const int width = c->geometry.width;
		const int resize_pos=width-AR_RESIZE_DELTA;
		const int shade_pos=width-AR_SHADE_DELTA;
#else /* ! TITLEBAR */
		const int resize_pos=(c->geometry.width-arwm.options.bw);
#endif

		XRaiseWindow(arwm.X.dpy, c->parent);
#ifdef TITLEBAR
		/* Text for close button press.  */
		if((x < AR_BUTTON_WIDTH) && (y < TITLEBAR_HEIGHT)) 
			send_wm_delete(c, !(e->state & arwm.keymasks.alt));
#endif /* TITLEBAR */
		if(x > resize_pos)
			sweep(c); /* Resize the window.  */
#ifdef TITLEBAR
		/* This implements window shading, a substitute for
		   iconification.  */
		else if(x > shade_pos && y < TITLEBAR_HEIGHT) 
		{
			/* Unshade.  */
			if(c->geometry.height==TITLEBAR_HEIGHT)
			{
				c->geometry.height=c->shade_height;
				c->flags &= ~AR_CLIENT_SHADED;
				moveresize(c);
			}
			else /* Shade.  */
			{
				c->shade_height=c->geometry.height;
				XResizeWindow(arwm.X.dpy, c->parent, 
					c->geometry.width, TITLEBAR_HEIGHT);
				c->geometry.height=TITLEBAR_HEIGHT;
				c->flags |= AR_CLIENT_SHADED;
			}
		}
#endif /* TITLEBAR */
		else
			drag(c);  /* Move the window.  */
		break;
	}
	case Button2: 
		XLowerWindow(arwm.X.dpy, c->parent); 
		break;
	case Button3:
		/* Resize operations more useful here,
		   rather than for third button, for laptop
		   users especially, where it is difficult
		   to register a middle button press, even
		   with X Emulate3Buttons enabled.  */
		sweep(c); 
		break;
#ifdef SCROLL_WHEEL_PAGER
	case Button4:
		move_client_with_vdesk(c, True);
		break;
	case Button5:
		move_client_with_vdesk(c, False);
		break;
#endif
	default: 
                LOG_DEBUG("Button event fell through to DEFAULT\n");
	}
}

