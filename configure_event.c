/* Copyright 2008-2011, Jeffrey Bedard <antiright@gmail.com> */

/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include "jbwm.h"

#if 0
static void
configure_client(Client * c, XConfigureRequestEvent * e)
{
	XWindowChanges wc;
	XRectangle *g = &(c->geometry);
	unsigned int value_mask = e->value_mask;
	const ubyte border = c->border;

#define ISV(cg) value_mask & CW##cg
#define MSKGEO(cg, geo) if(ISV(cg)) g->geo=e->geo;
	MSKGEO(Width, width);
	MSKGEO(Height, height);
	MSKGEO(X, x);
	MSKGEO(Y, y);
	if(ISV(StackMode) && ISV(Sibling))
	{
		Client *sibling;

		if((sibling = find_client(e->above)))
			wc.sibling = sibling->parent;
	}
	wc.x = g->x - border;
	wc.y = g->y - border;
	wc.border_width = border;
#if 0
	if(!(c->flags & JB_CLIENT_SHAPED))
	{

		value_mask |= CWHeight | CWY;
		wc.height += TITLEBAR_HEIGHT;
		wc.y -= TITLEBAR_HEIGHT;
#endif
	//}
	XConfigureWindow(jbwm.X.dpy, c->parent, value_mask, &wc);
	moveresize(c);
}
#endif

void
jbwm_handle_configure_request(XConfigureRequestEvent * e)
{
	Client *c;
	XWindowChanges wc;

	c=find_client(e->window);
	wc.x=e->x;
	wc.y=e->y;
	wc.width=e->width;
	wc.height=e->height;
	wc.border_width=0;
	wc.sibling=e->above;
	wc.stack_mode=e->detail;
	if(c)
	{
		if(e->value_mask & CWStackMode && e->value_mask & CWSibling)
		{
			Client *s;

			s = find_client(e->above);
			if(s)
				wc.sibling=s->parent;
		}
	}
	else
	{
		XConfigureWindow(jbwm.X.dpy, e->window, e->value_mask, &wc);
	}
#if 0
	XWindowChanges wc;
	Client *c;

	LOG("handle_configure_request(e);");
	wc.sibling = e->above;
	wc.stack_mode = e->detail;
	wc.height = e->height;
	wc.width = e->width;
	if((c = find_client(e->window)))
		configure_client(c, e);
	else
	{
		wc.x = e->x;
		wc.y = e->y;
		wc.width = e->width;
		wc.height = e->height;
#ifdef DEBUG
		fprintf(stderr, "wx:%d\twy:%d\tww:%d\twh:%d\n", wc.x,
			wc.y, wc.width, wc.height);
#endif /* DEBUG */
		if(wc.width == wc.height)
			return;
		XConfigureWindow(e->display, e->window, e->value_mask,
			&wc);
	}
#endif
}
