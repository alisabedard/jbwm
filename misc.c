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
#include "log.h"

int need_client_tidy = 0;

void 
handle_signal(int signo)
{
	(void)signo;	/* unused */
	XSetInputFocus(arwm.X.dpy, PointerRoot, 
		RevertToPointerRoot, CurrentTime);
	if (arwm.X.font)
		XftFontClose(arwm.X.dpy, arwm.X.font);
	{
		int i;

		for (i = 0; i < arwm.X.num_screens; i++)
		{
			XFreeGC(arwm.X.dpy, arwm.X.screens[i].invert_gc);
			XInstallColormap(arwm.X.dpy, 
					DefaultColormap(arwm.X.dpy, i));
		}
	}
#ifdef TITLEBAR
	$((&arwm.titlebar), delete);
#endif /* TITLEBAR */
	free(arwm.X.screens);
	exit(0);
}

int 
handle_xerror(Display * dsply, XErrorEvent * e)
{
	Client *c;

	(void)dsply;	/* unused */

	/* Ignore set as default action for nonmatched.  */

	/*
	 * If this error actually occurred while setting up the new window,
	 * best let make_new_client() know not to bother
	 */
	if (arwm.initialising != None && e->resourceid 
		== arwm.initialising)
	{
		LOG_DEBUG("\t **SAVED?** handle_xerror() caught error %d while initialising\n", e->error_code);
		arwm.initialising = None;
		return 0;
	}
	else if (e->error_code == BadAccess
		&& e->request_code == X_ChangeWindowAttributes)
	{
		LOG_ERROR("root window unavailable"
			" (maybe another wm is running?)\n");
		exit(1);
	}
	else if((c = find_client(e->resourceid)))
	{	
		/* This is used for nonmanaged windows that are not mapped
		   with OverrideRedirect being set.  This is for wm-spec
		   conformance.  */
		if(c->flags & AR_CLIENT_DONT_MANAGE)
		{
			XMapWindow(arwm.X.dpy, c->window);
			return 0;
		}
		LOG_DEBUG("\thandle_xerror() : flagging client for removal\n");
		c->flags &= ~AR_CLIENT_REMOVE;
		need_client_tidy = 0;
	}
	return 0; /* Ignore */
}
