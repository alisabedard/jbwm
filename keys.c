// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <X11/keysymdef.h>
#include <X11/Xlib.h>
#include "client.h"
#include "config.h"
#include "jbwmenv.h"
#include "keys.h"
#include "log.h"
#include "screen.h"
#include "ScreenInfo.h"
#include "snap.h"
#include "titlebar.h"

static void point(Client * c, const int x, const int y)
{
	XRaiseWindow(jbwm.dpy, c->parent);
	XWarpPointer(jbwm.dpy, None, c->window, 0, 0, 0, 0, x, y);
}

static void
keymv(Client * c, XKeyEvent * e, int *xy, int *wh, const int8_t sign)
{
	/* These operations invalid when maximized.  */
	if (c->flags & JB_MAXIMIZED)
		return;

	const int8_t d = sign * JBWM_RESIZE_INCREMENT;

	if ((e->state & jbwm.keymasks.mod) && (*wh > 0)) {
#ifdef USE_SHAPE

		if (c->flags & JB_SHAPED)
			goto move;

#endif//USE_SHAPE
		*wh += d;
	} else
#ifdef USE_SHAPE
 move:
#endif//USE_SHAPE
		*xy += d;

#ifdef USE_SNAP
	snap_border(c);
#endif//USE_SNAP
	moveresize(c);
	point(c, 1, 1);
}

static void handle_client_key_event(XKeyEvent * e, Client * c, KeySym key)
{
	XSizeHints *g = &(c->size);

	switch (key) {
	case KEY_LEFT:
		keymv(c, e, &(g->x), &(g->width), -1);
		break;

	case KEY_DOWN:
		keymv(c, e, &(g->y), &(g->height), 1);
		break;

	case KEY_UP:
		keymv(c, e, &(g->y), &(g->height), -1);
		break;

	case KEY_RIGHT:
		keymv(c, e, &(g->x), &(g->width), 1);
		break;

	case KEY_KILL:
		send_wm_delete(c);
		break;

	case KEY_LOWER:
	case KEY_ALTLOWER:
		XLowerWindow(jbwm.dpy, c->parent);
		break;

	case KEY_RAISE:
		XRaiseWindow(jbwm.dpy, c->parent);
		break;

	case KEY_FS:
		fullscreen(c);
		break;

	case KEY_MAX:
		maximize(c);
		break;

	case KEY_MAX_H:
		if(c->flags&JB_MAX_HORZ)
			  restore_horz(c);
		else
			  maximize_horz(c);
		break;

	case KEY_MAX_V:
		if(c->flags&JB_MAX_VERT)
			  restore_vert(c);
		else
			  maximize_vert(c);
		break;

	case KEY_STICK:
		stick(c);
		break;

	case KEY_MOVE:
		drag(c);
		break;

	case KEY_SHADE:
		shade(c);
	}
}

static void next(void)
{
	LOG("next()");
	Client *c = jbwm.current;

	do {
		if (c) {
			c = c->next;

			if (!c && !jbwm.current)
				return;
		}

		if (!c)
			c = jbwm.head;

		if (!c || (c == jbwm.current))
			return;

	} while (c->vdesk != c->screen->vdesk);

	if (!c)
		return;

	unhide(c);
	select_client(c);
	point(c, 0, 0);
	point(c, c->size.width, c->size.height);
}

static void
cond_client_to_desk(Client * c, ScreenInfo * s, const uint8_t d, const bool mod)
{
	LOG("mod: %d, c valid? %d\n", mod, c ? 1 : 0);

	if (mod && c)
		client_to_vdesk(c, d);
	else
		switch_vdesk(s, d);
}

void jbwm_handle_key_event(XKeyEvent * restrict e)
{
	const KeySym key = XLookupKeysym(e, 0);
	Client *c = jbwm.current;
	ScreenInfo *s = c ? c->screen : jbwm.screens;
	const bool mod = e->state & jbwm.keymasks.mod;
	bool zero_desk = false;
	uint8_t new_desk;

	switch (key) {
	case KEY_NEW:
		system(TERMINAL_CMD);
		break;

	case KEY_QUIT:
		exit(0);

	case KEY_NEXT:
		next();
		break;

	case XK_0:
		zero_desk = true;

	case XK_1:
	case XK_2:
	case XK_3:
	case XK_4:
	case XK_5:
	case XK_6:
	case XK_7:
	case XK_8:
	case XK_9:
		// First desktop 0, per wm-spec
		new_desk = zero_desk ? 10 : key - XK_1;
		cond_client_to_desk(c, s, new_desk, mod);
		break;

	case KEY_PREVDESK:
		new_desk = s->vdesk - 1;
		cond_client_to_desk(c, s, new_desk, mod);
		break;

	case KEY_NEXTDESK:
		new_desk = s->vdesk + 1;
		cond_client_to_desk(c, s, new_desk, mod);
		break;

	default:
		if (c)
			handle_client_key_event(e, c, key);
	}
}

static void
grab(ScreenInfo * restrict s, KeySym * restrict ks, const unsigned int mask)
{
	for (; *ks; ks++)
		XGrabKey(jbwm.dpy, XKeysymToKeycode(jbwm.dpy, *ks),
			 jbwm.keymasks.grab | mask, s->root, true,
			 GrabModeAsync, GrabModeAsync);
}

void grab_keys_for_screen(ScreenInfo * restrict s)
{
	grab(s, (KeySym[]){JBWM_KEYS_TO_GRAB}, 0);
	grab(s, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, jbwm.keymasks.mod);
}
