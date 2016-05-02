// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "keys.h"

#include <stdlib.h>
#include "client.h"
#include "config.h"
#include "jbwmenv.h"
#include "log.h"
#include "screen.h"
#include "snap.h"
#include "titlebar.h"

static void point(Client * restrict c, const int16_t x, const int16_t y)
{
	XRaiseWindow(jbwm.dpy, c->parent);
	XWarpPointer(jbwm.dpy, None, c->window, 0, 0, 0, 0, x, y);
}

static void
keymv(Client * c, const bool mod, int * restrict xy,
	int * restrict wh, const int8_t sign)
{
	/* These operations invalid when fullscreen.  */
	if (c->opt.fullscreen) return;

	const int8_t d = sign * JBWM_RESIZE_INCREMENT;

	if (mod && (*wh > 0)) {
		if (c->opt.shaped) goto move;
		*wh += d;
	} else
 move:
		*xy += d;
	snap_border(c);
	moveresize(c);
	point(c, 1, 1);
}

static void handle_client_key_event(const bool mod,
	Client * c, const KeySym key)
{
	switch (key) {
	case KEY_LEFT:
		keymv(c, mod, &(c->size.x), &(c->size.width), -1);
		break;
	case KEY_DOWN:
		keymv(c, mod, &(c->size.y), &(c->size.height), 1);
		break;
	case KEY_UP:
		keymv(c, mod, &(c->size.y), &(c->size.height), -1);
		break;
	case KEY_RIGHT:
		keymv(c, mod, &(c->size.x), &(c->size.width), 1);
		break;
	case KEY_KILL: send_wm_delete(c); break;
	case KEY_LOWER:
	case KEY_ALTLOWER: XLowerWindow(jbwm.dpy, c->parent); break;
	case KEY_RAISE: XRaiseWindow(jbwm.dpy, c->parent); break;
	case KEY_FS:
		if (c->opt.no_max) return;
		c->opt.is_fullscreen?unset_fullscreen(c):set_fullscreen(c);
		break;
	case KEY_MAX:
		// Honor !MWM_FUNC_MAXIMIZE
		// Maximizing shaped windows is buggy, so return.
		if(c->opt.shaped || c->opt.no_max) return;
		if(c->opt.max_horz && c->opt.max_vert) {
			unset_horz(c);
			unset_vert(c);
		} else {
			set_horz(c);
			set_vert(c);
		}
		break;
	case KEY_MAX_H:
		c->opt.max_horz?unset_horz(c):set_horz(c);
		break;
	case KEY_MAX_V:
		c->opt.max_vert?unset_vert(c):set_vert(c);
		break;
	case KEY_STICK: stick(c); break;
	case KEY_MOVE: drag(c); break;
	case KEY_SHADE: shade(c); break;
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
		if (!c) c = jbwm.head;
		if (!c || (c == jbwm.current))
			return;
	} while (c->vdesk != c->screen->vdesk);
	if(c) {
		unhide(c);
		select_client(c);
		point(c, 0, 0);
		point(c, c->size.width, c->size.height);
	}
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

void jbwm_handle_key_event(XKeyEvent * e)
{
	const KeySym key = XLookupKeysym(e, 0);
	Client *c = jbwm.current;
	ScreenInfo *s = c ? c->screen : jbwm.screens;
	const bool mod = e->state & jbwm.keymasks.mod;
	bool zero_desk = false;

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
	case XK_1: case XK_2: case XK_3: case XK_4: case XK_5: case XK_6:
	case XK_7: case XK_8: case XK_9:
		// First desktop 0, per wm-spec
		cond_client_to_desk(c, s, zero_desk ? 10 : key - XK_1, mod);
		break;

	case KEY_PREVDESK:
		cond_client_to_desk(c, s, s->vdesk - 1, mod);
		break;

	case KEY_NEXTDESK:
		cond_client_to_desk(c, s, s->vdesk + 1, mod);
		break;

	default:
		if (c) handle_client_key_event(e->state
			& jbwm.keymasks.mod, c, key);
	}
}

__attribute__((nonnull(1,2)))
static void
grab(ScreenInfo * restrict s, KeySym * restrict ks, const uint32_t mask)
{
	while(*(ks++))
		XGrabKey(jbwm.dpy, XKeysymToKeycode(jbwm.dpy, *ks),
			 jbwm.keymasks.grab | mask, s->root, true,
			 GrabModeAsync, GrabModeAsync);
}

void grab_keys_for_screen(ScreenInfo * restrict s)
{
	grab(s, (KeySym[]){JBWM_KEYS_TO_GRAB}, 0);
	grab(s, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, jbwm.keymasks.mod);
}

