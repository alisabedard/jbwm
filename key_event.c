/* Copyright 2008, Jeffrey Bedard <antiright@gmail.com> */

/* arwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <arwm@6809.org.uk>
 * see README for license and other details. */

#include "arwm.h"

static void
key_moveresize_client(Client * c)
{
	moveresize(c);
	setmouse(c->window, c->geometry.width + c->border - 1,
			c->geometry.height + c->border - 1);
	discard_enter_events();
}

static void
handle_client_key_event(XKeyEvent * e, Client * c, KeySym key)
{
	switch (key) 
	{
		case KEY_LEFT:
		{
			if((e->state & arwm.keymasks.alt) 
				&& (c->geometry.width 
				> ARWM_RESIZE_INCREMENT))
			{
				c->geometry.width -= ARWM_RESIZE_INCREMENT;
			}
			else
			{
				c->geometry.x -= ARWM_RESIZE_INCREMENT;
			}
			key_moveresize_client(c);
			break;
		}
		case KEY_DOWN:
		{
			if(e->state & arwm.keymasks.alt)
			{
				c->geometry.height+=ARWM_RESIZE_INCREMENT;
			}
			else
			{
				c->geometry.y += ARWM_RESIZE_INCREMENT;
			}
			key_moveresize_client(c);
			break;
		}
		case KEY_UP:
		{
			if((e->state & arwm.keymasks.alt)
				 && (c->geometry.height 
				 > ARWM_RESIZE_INCREMENT))
			{
				c->geometry.height -= ARWM_RESIZE_INCREMENT;
			}
			else 
			{
				c->geometry.y -= ARWM_RESIZE_INCREMENT;
			}
			key_moveresize_client(c);
			break;
		}
		case KEY_RIGHT:
		{
			if (e->state & arwm.keymasks.alt)
			{
				c->geometry.width+=ARWM_RESIZE_INCREMENT;
			}
			else 
			{
				c->geometry.x += ARWM_RESIZE_INCREMENT;
			}
			key_moveresize_client(c);
			break;
		}
		case KEY_TOPLEFT:
		{
			c->geometry.x = c->geometry.y = c->border;
			key_moveresize_client(c);
			break;
		}
		case KEY_TOPRIGHT:
		{
			c->geometry.x = 
				DisplayWidth(arwm.X.dpy, c->screen->screen)
				- c->geometry.width-c->border;
			c->geometry.y = c->border;
			key_moveresize_client(c);
			break;
		}
		case KEY_BOTTOMLEFT:
		{
			c->geometry.x = c->border;
			c->geometry.y = 
				DisplayHeight(arwm.X.dpy, c->screen->screen)
				- c->geometry.height-c->border;
			key_moveresize_client(c);
			break;
		}
		case KEY_BOTTOMRIGHT:
		{
			c->geometry.x = 
				DisplayWidth(arwm.X.dpy, c->screen->screen)
				- c->geometry.width-c->border;
			c->geometry.y = 
				DisplayHeight(arwm.X.dpy, c->screen->screen)
				- c->geometry.height-c->border;
			key_moveresize_client(c);
			break;
		}
		case KEY_KILL:
		{
			send_wm_delete(c, !(e->state & arwm.keymasks.alt));
			break;
		}
		case KEY_LOWER: case KEY_ALTLOWER:
		{
			XLowerWindow(arwm.X.dpy, c->parent);
			break;
		}
		case KEY_MAX:
		{
			maximize(c, MAXIMIZE_HORZ|MAXIMIZE_VERT);
			break;
		}
		case KEY_MAXVERT:
		{
			maximize(c, MAXIMIZE_VERT);
			break;
		}
		case KEY_FIX:
		{
			fix_client(c);
			break;
		}
	}
}

static void
handle_KEY_NEXT(XKeyEvent * e)
{
	next();
	if (XGrabKeyboard(arwm.X.dpy, e->root, False, GrabModeAsync, 
		GrabModeAsync, CurrentTime) == GrabSuccess) 
	{
		XEvent ev;

		do {
			XMaskEvent(arwm.X.dpy, KeyPressMask
				|KeyReleaseMask, &ev);
			if (ev.type == KeyPress 
				&& XKeycodeToKeysym(arwm.X.dpy,
				ev.xkey.keycode,0) 
				== KEY_NEXT)
				next();
		} while (ev.type == KeyPress 
			|| XKeycodeToKeysym(arwm.X.dpy,
			ev.xkey.keycode,0) == KEY_NEXT);
		XUngrabKeyboard(arwm.X.dpy, CurrentTime);
	}
	arwm_current_to_head();
}

#define SYSTEM(c) if(system(c)==-1) perror("unable to execute command")

void 
arwm_handle_key_event(XKeyEvent *e) 
{
	KeySym key = XKeycodeToKeysym(arwm.X.dpy,e->keycode,0);
	ScreenInfo *current_screen = find_current_screen();

	switch(key) {
		case KEY_NEW:
		{
			char * command;
					
			if(asprintf(&command, "%s &", opt_term[0])==-1)
				perror(	"unable to combine string "
					"at arwm_handle_key_event");
			SYSTEM(command);
			free(command);
		}
			break;
		case KEY_XLOCK:
			SYSTEM("xlock");
			break;
		case KEY_NEXT:
			handle_KEY_NEXT(e);
			break;
		case XK_1: case XK_2: case XK_3: case XK_4:
		case XK_5: case XK_6: case XK_7: case XK_8:
			switch_vdesk(current_screen, KEY_TO_VDESK(key));
			break;
		case KEY_PREVDESK:
			if (current_screen->vdesk > KEY_TO_VDESK(XK_1)) 
				switch_vdesk(current_screen,
					current_screen->vdesk - 1);
			break;
		case KEY_NEXTDESK:
			if (current_screen->vdesk < KEY_TO_VDESK(XK_8)) 
				switch_vdesk(current_screen,
					current_screen->vdesk + 1);
			break;
		default:
			if(current) 
				handle_client_key_event(e, current, key);
	}
}


