/* Copyright 2008-2011, Jeffrey Bedard <jefbed@gmail.com> */
/* jbwm - Minimalist Window Manager for X
 * Copyright (C) 1999-2006 Ciaran Anscomb <jbwm@6809.org.uk>
 * see README for license and other details. */

#include "jbwm.h"

static void
key_moveresize(Client * c)
{
	const ubyte in = c->border;

	moveresize(c);
	XRaiseWindow(jbwm.X.dpy, c->parent);
	setmouse(c->window, c->geometry.width - in,
		c->geometry.height - in);
}

static void
moveresize_dir(Client * c, XKeyEvent * e, short *xy,
	unsigned short *wh, const byte sign)
{
	const ubyte inc = ARWM_RESIZE_INCREMENT;
	const byte mod = sign * inc;

	if((e->state & jbwm.keymasks.mod) && (*wh > inc))
		*wh += mod;
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
	case KEY_FIX:
		fix_client(c);
		break;
	case KEY_MOVE:
		drag(c);
	}
}

static char *
bg_cmd(const char *command)
{
	char *buf = malloc(strlen(command) + 1);

	sprintf(buf, "%s&", command);

	return buf;
}

static void
jbwm_system(const char *exec)
{
	char *command;

	command = bg_cmd(exec);
	if(system(command))
		fprintf(stderr, "Failed to execute command: %s\n",
			exec);
	free(command);
}

#define spawn_terminal() jbwm_system(opt.term)

void
jbwm_handle_key_event(XKeyEvent * e)
{
	//KeySym key = XKeycodeToKeysym(jbwm.X.dpy, e->keycode, 0);
	KeySym key = XLookupKeysym(e, 0);

	Client *c = find_client(e->window);
	ScreenInfo *screen = c ? c->screen : jbwm.X.screens;

	switch (key)
	{
	case KEY_NEW:
		spawn_terminal();
		break;
	case KEY_QUIT:
		exit(0);
	case KEY_LOCK:
		if(system(XLOCK_CMD))
			perror(XLOCK_CMD);
		break;
	case KEY_BROWSER:
		if(system(BROWSER_CMD))
			perror(BROWSER_CMD);
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
