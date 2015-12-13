/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include <X11/Xproto.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "jbwm.h"

int
handle_xerror(Display * dpy, XErrorEvent * e)
{
	switch (e->error_code)
	{
	case BadAccess:
		if(e->request_code == X_ChangeWindowAttributes)
			ERROR("RootWindowTaken");
		break;
	case BadColor:
		LOG("BadColor");
		break;
	case BadValue:
		LOG("BadValue");
		break;
	case BadWindow:
		LOG("BadWindow");
		break;
	case BadDrawable:
		LOG("BadDrawable");
		break;
	default:
	{
		Client *c;
		
		LOG("Error # %d\n", e->error_code);
		if((c = find_client(e->resourceid)))
		{
			/* This is used for nonmanaged windows,
			 * that are not mapped, with OverrideRedirect 
			 * being set.  This is for wm-spec conformance.  */
			if(c->flags & JB_CLIENT_DONT_MANAGE)
			{
				LOG("JB_CLIENT_DONT_MANAGE");
				XMapWindow(dpy, c->window);
				return 0;
			}
			LOG("handle_xerror() : flag for removal");
			remove_client(c);
		}

	}
	}

	return 0;	/* Ignore */
}
