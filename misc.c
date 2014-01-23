/*
 * arwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <arwm@6809.org.uk> see README for license and other details.
 */

#include <X11/Xproto.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "arwm.h"

__attribute__ ((noreturn))
void
arwm_exit(const int status)
{
	if(head_client)
	while(head_client)
		remove_client(head_client);
	XSetInputFocus(arwm.X.dpy, PointerRoot, RevertToPointerRoot,
		CurrentTime);
	if(arwm.X.font)
#ifdef USE_XFT
		XftFontClose(arwm.X.dpy, arwm.X.font);
#else /* ! USE_XFT */
		XFreeFont(arwm.X.dpy, arwm.X.font);
#endif /* USE_XFT */
	while(arwm.X.num_screens--)
	{
		XFreeGC(arwm.X.dpy, 
			arwm.X.screens[arwm.X.num_screens].gc);
		XInstallColormap(arwm.X.dpy, DefaultColormap(arwm.X.dpy, 
			arwm.X.num_screens));
	}
#ifdef USE_TBAR
	ARWMTitlebarData_delete(&arwm.titlebar);
#endif
	free(arwm.X.screens);
	XFreeCursor(arwm.X.dpy, arwm.X.cursor);
	XCloseDisplay(arwm.X.dpy);

	exit(status);
}

int
handle_xerror(Display * dpy, XErrorEvent * e)
{
	/* If this error actually occurred while setting up the new window,
	 * best let make_new_client() know not to bother */
	if(arwm.initialising != None
		&& e->resourceid == arwm.initialising)
	{
		arwm.initialising = None;
		return 0;
	}
	switch(e->error_code)
	{
	case BadAccess:
		if(e->request_code == X_ChangeWindowAttributes)
			LOG_ERROR("root window unavailable\n");
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
			Client * c;

#ifdef DEBUG
			fprintf(stderr, "Error # %d\n", e->error_code);
#endif /* DEBUG */
			if((c = find_client(e->resourceid)))
			{
			/* This is used for nonmanaged windows 
			 * that are not mapped with OverrideRedirect 
			 * being set.  This is for wm-spec conformance.  */
				if(c->flags & AR_CLIENT_DONT_MANAGE)
				{
					XMapWindow(dpy, c->window);
					return 0;
				}
				LOG("handle_xerror() : flag for removal");
				remove_client(c);
				arwm.flags &= ~ARWM_FLAG_NEED_TIDY;
			}
	
		}
	}

	return 0;	/* Ignore */
}

