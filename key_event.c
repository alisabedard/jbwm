/* Copyright 2008-2011, Jeffrey Bedard <jefbed@gmail.com> */
/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include "jbwm.h"

static void
key_moveresize(Client * c)
{
	moveresize(c);
	XRaiseWindow(jbwm.X.dpy, c->parent);
	setmouse(c->window, JBWM_RESIZE_INCREMENT, JBWM_RESIZE_INCREMENT);
}

static void
moveresize_dir(Client * c, XKeyEvent * e, short *xy,
	unsigned short *wh, const byte sign)
{
	const ubyte inc = JBWM_RESIZE_INCREMENT;
	const byte mod = sign * inc;

	if((e->state & jbwm.keymasks.mod) && (*wh > inc))
	{
#ifdef USE_SHAPE
		if(is_shaped(c))
			return;
#endif /* USE_SHAPE */
		*wh += mod;
	}
	else
		*xy += mod;
	key_moveresize(c);
}

static void
handle_client_key_event(XKeyEvent * e, Client * c, KeySym key)
{
	XRectangle *g = &(c->geometry);

	switch (key)
	{
	case KEY_LEFT:
		moveresize_dir(c, e, &(g->x), &(g->width), -1);
		break;
	case KEY_DOWN:
		moveresize_dir(c, e, &(g->y), &(g->height), 1);
		break;
	case KEY_UP:
		moveresize_dir(c, e, &(g->y), &(g->height), -1);
		break;
	case KEY_RIGHT:
		moveresize_dir(c, e, &(g->x), &(g->width), 1);
		break;
	case KEY_TOPLEFT:
		g->x = g->y = c->border;
		key_moveresize(c);
		break;
	case KEY_TOPRIGHT:
		g->x = jbwm.X.screens->width - g->width - c->border;
		g->y = c->border;
		key_moveresize(c);
		break;
	case KEY_BOTTOMLEFT:
		g->x = c->border;
		g->y = jbwm.X.screens->height - g->height - c->border;
		key_moveresize(c);
		break;
	case KEY_BOTTOMRIGHT:
		g->x = jbwm.X.screens->width - g->width - c->border;
		g->y = jbwm.X.screens->height - g->height - c->border;
		key_moveresize(c);
		break;
	case KEY_KILL:
		send_wm_delete(c, !(e->state & jbwm.keymasks.mod));
		break;
	case KEY_LOWER:
	case KEY_ALTLOWER:
		XLowerWindow(jbwm.X.dpy, c->parent);
		break;
	case KEY_RAISE:
		XRaiseWindow(jbwm.X.dpy, c->parent);
		break;
	case KEY_MAX:
		maximize(c);
		break;
	case KEY_FIX:
		fix_client(c);
		break;
	case KEY_MOVE:
		drag(c);
	}
}

void
jbwm_handle_key_event(XKeyEvent * e)
{
	KeySym key = XLookupKeysym(e, 0);

	Client *c = find_client(e->window);
	ScreenInfo *screen = c ? c->screen : jbwm.X.screens;

	switch (key)
	{
	case KEY_NEW:
		system(opt.term);
		break;
	case KEY_QUIT:
		exit(0);
	case KEY_LOCK:
		system(XLOCK_CMD);
		break;
	case KEY_BROWSER:
		system(BROWSER_CMD);
		break;
	case KEY_NEXT:
		next();
		break;
	case XK_1:
	case XK_2:
	case XK_3:
	case XK_4:
	case XK_5:
	case XK_6:
	case XK_7:
	case XK_8:
	case XK_9:
		switch_vdesk(screen, KEY_TO_VDESK(key));
		break;
	case KEY_PREVDESK:
		if(screen && screen->vdesk > KEY_TO_VDESK(XK_1))
			switch_vdesk(screen, screen->vdesk - 1);
		break;
	case KEY_NEXTDESK:
		if(screen && screen->vdesk < KEY_TO_VDESK(XK_8))
			switch_vdesk(screen, screen->vdesk + 1);
		break;
	default:
		if(current)
			handle_client_key_event(e, current, key);
	}
}
