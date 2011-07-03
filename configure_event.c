/* Copyright 2008, Jeffrey Bedard <antiright@gmail.com> */

/* arwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <arwm@6809.org.uk>
 * see README for license and other details. */

#include "arwm.h"

void 
arwm_handle_configure_request(XConfigureRequestEvent *e) 
{
	XWindowChanges wc;
	Client *c = find_client(e->window);
	unsigned int value_mask = e->value_mask;
	Display * dpy = arwm.X.dpy;

	LOG_DEBUG("handle_configure_request(e);\n");

	wc.sibling = e->above;
	wc.stack_mode = e->detail;
	wc.height = e->height;
	wc.width = e->width;
	if (c) {
		XRectangle * geometry = &(c->geometry);

		ungravitate(c);
		if (value_mask & CWWidth) geometry->width = e->width;
		if (value_mask & CWHeight) geometry->height = e->height;
		if (value_mask & CWX) geometry->x = e->x;
		if (value_mask & CWY) geometry->y = e->y;
		if (value_mask & CWStackMode && value_mask & CWSibling) 
		{
			Client *sibling = find_client(e->above);
			if (sibling) 
				wc.sibling = sibling->parent;
		}
		if (geometry->x == 0 && geometry->width 
			>= DisplayWidth(dpy, c->screen->screen)) 
			geometry->x -= c->border;
		if (geometry->y == 0 && geometry->height 
			>= DisplayHeight(dpy, c->screen->screen)) 
			geometry->y -= c->border;
		gravitate(c);
		wc.x = geometry->x - c->border;
		wc.y = geometry->y - c->border;
		wc.border_width = c->border;
		LOG_XDEBUG("%s:%d:XConfigureWindow(arwm.X.dpy, "
			"parent(%x), %x, &wc);\n", 
			__FILE__, __LINE__,
			(unsigned int)c->parent, value_mask);
#ifdef TITLEBAR
#ifdef SHAPE
		if(!(c->flags & AR_CLIENT_SHAPED))
		{
#endif
			value_mask |= CWHeight;
			wc.height += TITLEBAR_HEIGHT; 
			value_mask |= CWY;
			wc.y -= TITLEBAR_HEIGHT; 
#ifdef SHAPE
		}
#endif
#endif /* TITLEBAR */
		XConfigureWindow(dpy, c->parent, value_mask, &wc);
		moveresize(c);
	} 
	else 
	{
		wc.x = c ? 0 : e->x;
		wc.y = c ? 0 : e->y;
		MARK;
		LOG_XDEBUG("XConfigureWindow(dpy, window(%x), %x, &wc);\n", 
			   (unsigned int)e->window, value_mask);
		XConfigureWindow(dpy, e->window, value_mask, &wc);
	}
}


