/*
 * arwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <arwm@6809.org.uk> see README for license and other details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "arwm.h"
#include "log.h"


/*
 * used all over the place.  return the client that has specified window as
 * either window or parent
 */

inline Client *
find_client(Window w)
{
	Client *c;

	/* As this is called often, a vector may provide faster iteration.  */
	for (c = head_client;
		c && (w != c->parent) && (w != c->window)
#if defined(TITLEBAR)
	/*
	 * This allows the info_window member, which is the titlebar parent,
	 * to match events.  Particularaly, it allows the titlebar to be used
	 * as a drag handle.
	 */
		&& (w != c->info_window)
#endif	/* TITLEBAR */
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
	long data[2] = {state, None};

	XChangeProperty(arwm.X.dpy, c->window, arwm.atoms.wm.state, 
		arwm.atoms.wm.state, 32,
		PropModeReplace, (unsigned char *)data, 2);
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

inline void
send_config(Client * c)
{
	SET_CLIENT_CE(c);
	SEND_CONFIG(c);
}

#define XY_ASSIGN(c, X, Y)\
	c->geometry.x+=X;\
	c->geometry.y+=Y;

/*
 * Support for 'gravitating' clients based on their original border width and
 * configured window manager frame width.
 */
void 
gravitate_client(Client * c, int sign)
{
	int d0 = sign * c->border;
	int d1 = sign * c->old_border;
	int d2 = sign * (c->old_border + c->old_border - c->border);

	LOG_DEBUG("gravitate_client(c, s)\n");
	if(c->flags & AR_CLIENT_DONT_MANAGE)
	{
		LOG_DEBUG("\tbailing from gravitation...\n");
		return;
	}

	switch (c->win_gravity)
	{
	case NorthGravity:
		XY_ASSIGN(c, d1, d0);
		break;
	case NorthEastGravity:
		XY_ASSIGN(c, d2, d0);
		break;
	case EastGravity:
		XY_ASSIGN(c, d2, d1);
		break;
	case SouthEastGravity:
		XY_ASSIGN(c, d2, d2);
		break;
	case SouthGravity:
		XY_ASSIGN(c, d1, d2);
		break;
	case SouthWestGravity:
		XY_ASSIGN(c, d0, d2);
		break;
	case WestGravity:
		XY_ASSIGN(c, d0, d1);
		break;
	case NorthWestGravity:
	default:
		XY_ASSIGN(c, d0, d0);
		break;
	}
	SET_CLIENT_CE(c);
}

void 
select_client(Client * c)
{
	LOG_DEBUG("select_client(c);\n");
	/* Set inactive window border for CURRENT.  */
	if (current)
	{
          LOG_DEBUG("CURRENT is true\n");
		XSetWindowBorder(arwm.X.dpy, current->parent,
			current->screen->bg.pixel);
		current->flags &= ~AR_CLIENT_ACTIVE;
		update_info_window(current);
	}
	
	if (c)
	{
          LOG_DEBUG("C is true\n");
		c->flags |= AR_CLIENT_ACTIVE;
		XSetWindowBorder(arwm.X.dpy, c->parent, 
			(c->flags & AR_CLIENT_BLACK_BORDER) 
			? BlackPixel(arwm.X.dpy, c->screen->screen)
			: (is_sticky(c)
			? c->screen->fc.pixel
			: c->screen->fg.pixel));
		XInstallColormap(arwm.X.dpy, c->cmap);
		XSetInputFocus(arwm.X.dpy, c->window,
			RevertToPointerRoot, CurrentTime);
	        update_info_window(c);
	}
	current = c;
        update_info_window(current);
}

void 
fix_client(Client * c)
{
	toggle_sticky(c);
	select_client(c);
	ewm_update_net_wm_state(c);
}

static void
withdraw_window(Client * c)
{
	/*
	 * ICCCM 4.1.3.1 "When the window is withdrawn, the window manager
	 * will either change the state field's value to WithdrawnState or it
	 * will remove the WM_STATE property entirely." ARWMH 1.3 "The Window
	 * Manager should remove the property whenever a window is withdrawn
	 * but it should leave the property in place when it is shutting
	 * down." (both _NET_WM_DESKTOP and _NET_WM_STATE)
	 */
	if (c->flags & AR_CLIENT_REMOVE)
	{
		set_wm_state(c, WithdrawnState);
		XDeleteProperty(arwm.X.dpy, c->window, arwm.atoms.vwm.desktop);
		XDeleteProperty(arwm.X.dpy, c->window, arwm.atoms.vwm.state);
	}
}

static void
kill_parent_window(Client * c)
{
	XReparentWindow(arwm.X.dpy, c->window, c->screen->root, 
		c->geometry.x, c->geometry.y);
	XSetWindowBorderWidth(arwm.X.dpy, c->window, c->old_border);
	XRemoveFromSaveSet(arwm.X.dpy, c->window);
	if (c->parent)
		XDestroyWindow(arwm.X.dpy, c->parent);
}

static void
relink_window_list(Client * c)
{
	if (head_client == c)
		head_client = c->next;
	else
	{
		Client *p;

		for (p = head_client; p && p->next; p = p->next)
			if (p->next == c)
				p->next = c->next;
	}
}

void 
remove_client(Client * c)
{
	LOG_DEBUG("remove_client() : Removing...\n");

	XGrabServer(arwm.X.dpy);

#ifdef TITLEBAR
	remove_info_window(c);
#endif	/* TITLEBAR */

	withdraw_window(c);
	ungravitate(c);
	kill_parent_window(c);

	relink_window_list(c);

	if (current == c)
		current = NULL;	/* an enter event should set this up again */
	free(c);

	XUngrabServer(arwm.X.dpy);
	XSync(arwm.X.dpy, False);
	LOG_DEBUG("remove_client() returning\n");
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

	return XSendEvent(arwm.X.dpy, w, False, NoEventMask, &ev);
}

void 
send_wm_delete(Client * c, int kill_client)
{
	if(kill_client)
		send_xmessage(c->window, arwm.atoms.wm.protos, 
			arwm.atoms.wm.delete);
	else
		XKillClient(arwm.X.dpy, c->window);
}


void 
set_shape(Client * c)
{
	int bounding_shaped;
	int i, b;
	unsigned int u;	/* dummies */

	LOG_DEBUG("set_shape(c)\n");

	/* Validate inputs:  Make sure that the SHAPE extension is
	   available, and make sure that C is initialized.  */
	if (!arwm.X.have_shape || !c)
          {
            LOG_DEBUG("not arwm.X.have_shape || not c, so return\n");
		return;
          }
	/*
	 * Logic to decide if we have a shaped window cribbed from
	 * fvwm-2.5.10. Previous method (more than one rectangle returned
	 * from XShapeGetRectangles) worked _most_ of the time.
	 */
	XShapeSelectInput(arwm.X.dpy, c->window, ShapeNotifyMask);
	if (XShapeQueryExtents(arwm.X.dpy, c->window, &bounding_shaped, &i, &i,
			&u, &u, &b, &i, &i, &u, &u)
		&& bounding_shaped)
	{
		c->flags |= AR_CLIENT_SHAPED | AR_CLIENT_DONT_USE_TITLEBAR;
		XShapeCombineShape(arwm.X.dpy, c->parent, ShapeBounding, 0, 
#ifdef TITLEBAR
			TITLEBAR_HEIGHT +
#endif
			0,
			c->window, ShapeBounding, ShapeSet);

		LOG_DEBUG("\t c is SHAPED\n");
	}
}

