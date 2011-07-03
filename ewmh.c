/*
 * arwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <arwm@6809.org.uk> see README for license and other details.
 */

#include <stdlib.h>
#include "arwm.h"
#include "log.h"

void 
ewm_update_net_wm_state(Client * c)
{
	Atom state[1];
	int i = 0;

	if (is_sticky(c))
		state[i++] = arwm.atoms.vwm.sticky;
	ARWM_VWM_CHANGE_PROPERTY(c, XA_ATOM, (unsigned char *)&state, i);
}

void
arwm_setup_ewmh_hints(void)
{
	LOG_DEBUG("arwm_setup_ewmh_hints()\n");
	arwm.atoms.net_wm_window_type.desktop
		= GETATOM("_NET_WM_WINDOW_TYPE_DESKTOP");
	arwm.atoms.net_wm_window_type.dock
		= GETATOM("_NET_WM_WINDOW_TYPE_DOCK");
	arwm.atoms.net_wm_window_type.toolbar
		= GETATOM("_NET_WM_WINDOW_TYPE_TOOLBAR");
	arwm.atoms.net_wm_window_type.menu
		= GETATOM("_NET_WM_WINDOW_TYPE_MENU");
	arwm.atoms.net_wm_window_type.utility
		= GETATOM("_NET_WM_WINDOW_TYPE_UTILITY");
	arwm.atoms.net_wm_window_type.splash
		= GETATOM("_NET_WM_WINDOW_TYPE_SPLASH");
	arwm.atoms.net_wm_window_type.dialog
		= GETATOM("_NET_WM_WINDOW_TYPE_DIALOG");
	arwm.atoms.net_wm_window_type.dropdown_menu
		= GETATOM("_NET_WM_WINDOW_TYPE_DROPDOWN_MENU");
	arwm.atoms.net_wm_window_type.popup_menu
		= GETATOM("_NET_WM_WINDOW_TYPE_POPUP_MENU");
	arwm.atoms.net_wm_window_type.tooltip
		= GETATOM("_NET_WM_WINDOW_TYPE_TOOLTIP");
	arwm.atoms.net_wm_window_type.notification
		= GETATOM("_NET_WM_WINDOW_TYPE_NOTIFICATION");
	arwm.atoms.net_wm_window_type.combo
		= GETATOM("_NET_WM_WINDOW_TYPE_COMBO");
	arwm.atoms.net_wm_window_type.dnd
		= GETATOM("_NET_WM_WINDOW_TYPE_DND");
	arwm.atoms.net_wm_window_type.normal
		= GETATOM("_NET_WM_WINDOW_TYPE_NORMAL");
}

static void
arwm_handle_net_wm_type_hints(Client * c)
{
	unsigned char * type_prop;
	Atom type;
	ARWMNetWMWindowType * types = &(arwm.atoms.net_wm_window_type);
	unsigned long nitems;
	
	type_prop=arwm_get_property(c->window, 
		GETATOM("_NET_WM_WINDOW_TYPE"), XA_ATOM, &nitems);
	if(!type_prop)
	{
		/* This follows the freedesktop.org wm-spec guidline
		   for windows without the window type property set, but
		   with the transient_for property set, in which they
		   are assumed to be dialogs.  */
		if(arwm_get_property(c->window, 
			XA_WM_TRANSIENT_FOR,
			XA_ATOM, &nitems))
		{
			LOG_DEBUG("fallback to dialog");
			goto is_dialog;
		}
		return;
	}
	c->window_type=type=type_prop[0];

	if(type==types->normal)
	{
		LOG_DEBUG("normal\n");
	}
	else if(type==types->dialog)
	{
is_dialog:
		c->border=4;
		c->flags|=AR_CLIENT_BLACK_BORDER;
		LOG_DEBUG("dialog\n");
	}
	else if(type==types->splash)
	{
		/* FIXME:  Remove area that would have been consumed
		   by the titlebar.  */ 
		c->flags |= AR_CLIENT_DONT_USE_TITLEBAR | AR_CLIENT_SHAPED;
		c->border=0;
		LOG_DEBUG("splash\n");
	}
	else if(type==types->desktop)
	{
		/* Display on all desktops.  */
		add_sticky(c);
		/* Lowers the window, per the wm-spec.  */
		XLowerWindow(arwm.X.dpy, c->window);
		LOG_DEBUG("desktop\n");
	}
	else if(type==types->dock)
	{
		XWindowAttributes attributes;
		XSetWindowAttributes changes; 

		LOG_DEBUG("dock\n");

		add_sticky(c);
		/* Raise the window, per the wm-spec.  */
		XRaiseWindow(arwm.X.dpy, c->window);
#ifdef TITLEBAR
		remove_info_window(c);
#endif /* TITLEBAR */
		c->flags |= AR_CLIENT_DONT_MANAGE;
		XGetWindowAttributes(arwm.X.dpy, c->window, &attributes);
		LOG_DEBUG("x:%d\ty:%d\n", attributes.x, attributes.y);
		XReparentWindow(arwm.X.dpy, c->window, 
			c->screen->root,
			attributes.x, attributes.y);
		XDestroyWindow(arwm.X.dpy, c->parent);
		c->border=0;
		changes.override_redirect=True;
		changes.event_mask=None;
		XChangeWindowAttributes(arwm.X.dpy, c->window, 
			CWOverrideRedirect | CWEventMask, &changes);
		c->ignore_unmap--;
	}
	else 
	{
		LOG_DEBUG("unknown\n");
	}
}

static void
handle_urgency_hint(Client *c)
{
  XWMHints * std_hints;

  std_hints=XGetWMHints(arwm.X.dpy, c->window);
  if(std_hints)
    {
      const long flags = std_hints->flags;

      XFree(std_hints);
      if(flags & XUrgencyHint)
        XRaiseWindow(arwm.X.dpy, c->parent);
    }
}

static void
handle_urgency_property(Client *c)
{
	unsigned char * prop;
	Atom urgent=GETATOM("URGENT");
	Window w = c->window;
	unsigned long nitems;

	/* Raise URGENT windows.  */
	prop=arwm_get_property(w, urgent, XA_ATOM, &nitems);
	if(prop)
	{
		XFree(prop);
		XRaiseWindow(arwm.X.dpy, c->parent);
		XDeleteProperty(arwm.X.dpy, w, urgent);
	}
}

/* This should be used on events, not when mapping.  */
void
arwm_handle_net_wm_state_hints(Client * c)
{
	unsigned long nitems;
	unsigned char * state_prop 
          = arwm_get_property(c->window, arwm.atoms.vwm.state,
                              XA_ATOM, &nitems);
	Atom state=0;

	LOG_DEBUG("arwm_h_n_w_s_h(c)\n");

	if(state_prop)
	{
	  state=state_prop[0];
          XFree(state_prop);
	}
        if(state==arwm.atoms.vwm.fullscreen)
          MAXIMIZE(c);
        else if(state==arwm.atoms.vwm.sticky)
          add_sticky(c);

  handle_urgency_hint(c);
  handle_urgency_property(c);
}

void
arwm_handle_ewmh_hints(Client * c)
{
	arwm_handle_net_wm_type_hints(c);
	arwm_handle_net_wm_state_hints(c);
}

