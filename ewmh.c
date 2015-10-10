/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#ifdef USE_EWMH
#include <stdlib.h>
#include "jbwm.h"
#include "log.h"

/* FIXME: This does not actually work.  */

void
ewm_update_net_wm_state(Client * c)
{
	Atom state[1];
	int i = 0;

	if(is_sticky(c))
		state[i++] = ARWM_ATOM_VWM_STICKY;
	ARWM_VWM_CHANGE_PROPERTY(c, XA_ATOM, (unsigned char *)&state,
		i);
}

#if 0
void
jbwm_setup_ewmh_hints(void)
{
	LOG_DEBUG("jbwm_setup_ewmh_hints()\n");
	jbwm.atoms.netwm.desktop =
		GETATOM("_NET_WM_WINDOW_TYPE_DESKTOP");
	jbwm.atoms.netwm.dock = GETATOM("_NET_WM_WINDOW_TYPE_DOCK");
	jbwm.atoms.netwm.toolbar =
		GETATOM("_NET_WM_WINDOW_TYPE_TOOLBAR");
	jbwm.atoms.netwm.menu = GETATOM("_NET_WM_WINDOW_TYPE_MENU");
	jbwm.atoms.netwm.utility =
		GETATOM("_NET_WM_WINDOW_TYPE_UTILITY");
	jbwm.atoms.netwm.splash =
		GETATOM("_NET_WM_WINDOW_TYPE_SPLASH");
	jbwm.atoms.netwm.dialog =
		GETATOM("_NET_WM_WINDOW_TYPE_DIALOG");
	jbwm.atoms.netwm.dropdown_menu =
		GETATOM("_NET_WM_WINDOW_TYPE_DROPDOWN_MENU");
	jbwm.atoms.netwm.popup_menu =
		GETATOM("_NET_WM_WINDOW_TYPE_POPUP_MENU");
	jbwm.atoms.netwm.tooltip =
		GETATOM("_NET_WM_WINDOW_TYPE_TOOLTIP");
	jbwm.atoms.netwm.notification =
		GETATOM("_NET_WM_WINDOW_TYPE_NOTIFICATION");
	jbwm.atoms.netwm.combo = GETATOM("_NET_WM_WINDOW_TYPE_COMBO");
	jbwm.atoms.netwm.dnd = GETATOM("_NET_WM_WINDOW_TYPE_DND");
	jbwm.atoms.netwm.normal =
		GETATOM("_NET_WM_WINDOW_TYPE_NORMAL");
}
#endif

static unsigned char *
get_prop(Window w, Atom prop)
{
	Atom type;
	int format;
	unsigned long nitems;
	unsigned long bytes_after_return;
	unsigned char *data;

	XGetWindowProperty(jbwm.X.dpy, w, prop, 0L, 1024, False,
		AnyPropertyType, &type, &format, &nitems,
		&bytes_after_return, &data);

	return data;
}

static void
setup_dock(Client * c)
{
	XWindowAttributes attributes;
	XSetWindowAttributes changes;

	/* Raise the window, per the wm-spec.  */
	XRaiseWindow(jbwm.X.dpy, c->window);
	remove_info_window(c);
	c->flags |= AR_CLIENT_DONT_MANAGE;
	XGetWindowAttributes(jbwm.X.dpy, c->window, &attributes);
	XReparentWindow(jbwm.X.dpy, c->window, c->screen->root,
		attributes.x, attributes.y);
	XDestroyWindow(jbwm.X.dpy, c->parent);
	c->border = 0;
	changes.override_redirect = True;
	changes.event_mask = None;
	XChangeWindowAttributes(jbwm.X.dpy, c->window,
		CWOverrideRedirect | CWEventMask, &changes);
	c->ignore_unmap--;
}

void
jbwm_ewmh_type(Client * c)
{
	unsigned char *type_prop;
	Atom type;
	//ARWMNetWMWindowType *types = &(jbwm.atoms.netwm);
	unsigned long nitems;
	int ntypes = 0;
	Atom *atypes;

	LOG_DEBUG("jbwm_ewmh_type()\n") type_prop =
		get_prop(c->window, 331);
	atypes = XListProperties(jbwm.X.dpy, c->window, &ntypes);
	if(!atypes)
		return;
	for(; ntypes >= 0; ntypes--)
	{
		LOG_DEBUG("TYPES[%d]: %d\t", ntypes,
			(int)atypes[ntypes]);
		switch ((int)atypes[ntypes])
		{
		case 321:
			setup_dock(c);
			break;
		case 331:
			LOG_DEBUG("\t_NET_WM_WINDOW_TYPE\n");
			break;
		default:
			break;
		}
		if((Atom) atypes[ntypes] ==
			GETATOM("_NET_WM_WINDOW_TYPE_DOCK"))
			puts("MATCH\t\t\t*********");

	}
	if(!type_prop)
	{
		LOG_DEBUG("!type_prop\n");
		/* This follows the freedesktop.org wm-spec guidline
		   for windows without the window type property set, but
		   with the transient_for property set, in which they
		   are assumed to be dialogs.  */
		if(jbwm_get_property(c->window, XA_WM_TRANSIENT_FOR,
			XA_ATOM, &nitems))
		{
			LOG_DEBUG("fallback to dialog");
			goto is_dialog;
		}
		return;
	}
	c->window_type = type = type_prop[0];

	if(type == GETATOM("_NET_WM_WINDOW_TYPE_DIALOG"))
	{
		LOG_DEBUG("type==dialog\n");
	      is_dialog:
		c->border = 4;
		c->flags |= AR_CLIENT_BLACK_BORDER;
	}
	else if(type == GETATOM("_NET_WM_WINDOW_TYPE_SPLASH"))
	{
		LOG_DEBUG("type==splash\n");
		/* FIXME:  Remove area that would have been consumed
		   by the titlebar.  */
		c->flags |=
			AR_CLIENT_DONT_USE_TITLEBAR |
			AR_CLIENT_SHAPED;
		c->border = 0;
	}
	else if(type == GETATOM("_NET_WM_WINDOW_TYPE_SPLASH"))
	{
		LOG_DEBUG("type==desktop\n");
		/* Display on all desktops.  */
		add_sticky(c);
		/* Lowers the window, per the wm-spec.  */
		XLowerWindow(jbwm.X.dpy, c->window);
	}
	else if(type == GETATOM("_NET_WM_WINDOW_TYPE_SPLASH"))
		setup_dock(c);
	else
	{
		LOG_DEBUG("unknown: %d\n", (int)type);
	}
}

/* This should be used on events, not when mapping.  */
void
jbwm_ewmh_state(Client * c)
{
	XWMHints *hints;
	Window w = c->window;

	jbwm_ewmh_type(c);
	hints = XGetWMHints(jbwm.X.dpy, w);
	if(hints)
	{
		const long flags = hints->flags;
		XFree(hints);
		if(flags & XUrgencyHint)
			maximize(c);
	}
}
#endif /* USE_EWMH */
