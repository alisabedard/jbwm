// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "keys.h"

#include "client.h"
#include "config.h"
#include "jbwmenv.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "snap.h"
#include "titlebar.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

__attribute__((nonnull))
static void point(Client * restrict c, const int16_t x, const int16_t y)
{
	XRaiseWindow(jbwm.d, c->parent);
	XWarpPointer(jbwm.d, None, c->window, 0, 0, 0, 0, x, y);
}

__attribute__((nonnull))
static void keymv(Client * restrict c, int * restrict xy,
	int * restrict wh, const bool mod, const int8_t sign)
{
	/* These operations invalid when fullscreen.  */
	if (c->opt.fullscreen)
		return;
	const int8_t d = sign * JBWM_RESIZE_INCREMENT;

	*(mod && (*wh > JBWM_RESIZE_INCREMENT<<1)
		&& !c->opt.shaped && !c->opt.no_resize ? wh : xy) += d;
	snap_border(c);
	moveresize(c);
	point(c, 1, 1);
}

__attribute__((nonnull))
static void handle_client_key_event(const bool mod,
	Client * restrict c, const KeySym key)
{
	LOG("handle_client_key_event: %d", (int)key);
	switch (key) {
	case KEY_LEFT:
		keymv(c, &(c->size.x), &(c->size.width), mod, -1);
		break;
	case KEY_DOWN:
		keymv(c, &(c->size.y), &(c->size.height), mod, 1);
		break;
	case KEY_UP:
		keymv(c, &(c->size.y), &(c->size.height), mod, -1);
		break;
	case KEY_RIGHT:
		keymv(c, &(c->size.x), &(c->size.width), mod, 1);
		break;
	case KEY_KILL: send_wm_delete(c); break;
	case KEY_LOWER:
	case KEY_ALTLOWER: XLowerWindow(jbwm.d, c->parent); break;
	case KEY_RAISE: XRaiseWindow(jbwm.d, c->parent); break;
	case KEY_FS:
		if (!c->opt.no_max)
			(c->opt.fullscreen ? unset_fullscreen
			 : set_fullscreen)(c);
		break;
	case KEY_MAX: {
		// Honor !MWM_FUNC_MAXIMIZE
		// Maximizing shaped windows is buggy, so return.
		if(c->opt.shaped || c->opt.no_max)
			break;
		const bool max = c->opt.max_horz && c->opt.max_vert;
		(max ? unset_horz : set_horz)(c);
		(max ? unset_vert : set_vert)(c);
	}
		break;
	case KEY_MAX_H:
		(c->opt.max_horz ? unset_horz : set_horz)(c);
		break;
	case KEY_MAX_V:
		(c->opt.max_vert ? unset_vert : set_vert)(c);
		break;
	case KEY_STICK: stick(c); break;
	case KEY_MOVE: drag(c); break;
	case KEY_SHADE: shade(c); break;
	}
}

static Client * get_next_on_vdesk(void)
{
	Client *c = jbwm.current;
	do {
		if (c) {
			c = c->next;
			if (!c && !jbwm.current)
				break;
		}
		if (!c) c = jbwm.head;
		if (!c || (c == jbwm.current))
			break;
	} while (c->vdesk != c->screen->vdesk);
	return c;
}

static void next(void)
{
	Client * c = get_next_on_vdesk();
	if (!c)
		return;
	unhide(c);
	select_client(c);
	point(c, 0, 0);
	point(c, c->size.width, c->size.height);
}

static void cond_client_to_desk(Client * c, ScreenInfo * s,
	const uint8_t d, const bool mod)
{
	mod && c ? client_to_vdesk(c, d) : switch_vdesk(s, d);
}

void jbwm_handle_key_event(XKeyEvent * e)
{
	LOG("jbwm_handle_key_event");
	const KeySym key = XLookupKeysym(e, 0);
	Client *c = jbwm.current;
	ScreenInfo *s = c ? c->screen : jbwm.s;
	struct {
		uint8_t vdesk:4;
		bool mod:1;
		bool zero:1;
	} opt = {s->vdesk, e->state & jbwm.keymasks.mod, 0};

	switch (key) {
	case KEY_NEW: {
		const int r = system(TERMINAL_CMD);
		if (!WIFEXITED(r) || WEXITSTATUS(r)) {
			fputs("Could not execute terminal command\n",
				stderr);
		}
		break;
	}
	case KEY_QUIT:
		exit(0);
	case KEY_NEXT:
		next();
		break;
	case XK_0:
		opt.zero = true;
	case XK_1: case XK_2: case XK_3: case XK_4: case XK_5: case XK_6:
	case XK_7: case XK_8: case XK_9:
		// First desktop 0, per wm-spec
		cond_client_to_desk(c, s, opt.zero ? 10 : key - XK_1, opt.mod);
		break;
	case KEY_PREVDESK:
		cond_client_to_desk(c, s, s->vdesk - 1, opt.mod);
		break;
	case KEY_NEXTDESK:
		cond_client_to_desk(c, s, s->vdesk + 1, opt.mod);
		break;
	default:
		if (c)
			handle_client_key_event(opt.mod, c, key);
	}
}

__attribute__((nonnull(1,2)))
static void grab(ScreenInfo * restrict s, KeySym * restrict ks,
	const uint32_t mask)
{
	for (; *ks; ++ks)
		XGrabKey(jbwm.d, XKeysymToKeycode(jbwm.d, *ks),
			 jbwm.keymasks.grab | mask, s->root, true,
			 GrabModeAsync, GrabModeAsync);
}

void grab_keys_for_screen(ScreenInfo * restrict s)
{
	grab(s, (KeySym[]){JBWM_KEYS_TO_GRAB}, 0);
	grab(s, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, jbwm.keymasks.mod);
}

