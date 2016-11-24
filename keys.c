// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "keys.h"
#include "client.h"
#include "config.h"
#include "jbwm.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "snap.h"
#include "title_bar.h"
#include "util.h"
#include <stdlib.h>
#include <unistd.h>
//#define DEBUG_KEYS
#ifndef DEBUG_KEYS
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!DEBUG_KEYS
static struct {
	uint16_t grab_mask, mod_mask;
} jbwm_keys_data = { .grab_mask = JBWM_KEYMASK_GRAB,
	.mod_mask = JBWM_KEYMASK_MOD };
void jbwm_set_grab_mask(const uint16_t mask)
{
	jbwm_keys_data.grab_mask = mask;
}
void jbwm_set_mod_mask(const uint16_t mask)
{
	jbwm_keys_data.mod_mask = mask;
}
void jbwm_grab_window_keys(Display * restrict d, const jbwm_window_t win)
{
	jbwm_grab_button(d, win, jbwm_keys_data.grab_mask, AnyButton);
}
__attribute__((nonnull))
static void point(struct JBWMClient * restrict c,
	const int16_t x, const int16_t y)
{
	Display * restrict d = c->d;
	XRaiseWindow(d, c->parent);
	XWarpPointer(d, None, c->window, 0, 0, 0, 0, x, y);
}
__attribute__((nonnull))
static void commit_key_move(struct JBWMClient * restrict c)
{
	jbwm_snap_border(c);
	jbwm_move_resize(c);
	point(c, 1, 1);
}
struct KeyMoveFlags {
	bool horz:1, pos:1, mod:1;
	uint8_t pad:5;
};
__attribute__((nonnull))
static void key_move(struct JBWMClient * restrict c,
	const struct KeyMoveFlags f)
{
	const int8_t d = f.pos ? JBWM_RESIZE_INCREMENT
		: -JBWM_RESIZE_INCREMENT;
	struct JBWMRectangle * restrict s = &c->size;
	uint16_t * wh = f.horz ? &s->width : &s->height;
	int16_t * xy = f.horz ? &s->x : &s->y;
	if(f.mod && (*wh > JBWM_RESIZE_INCREMENT << 1)
		&& !c->opt.shaped && !c->opt.no_resize)
		*wh += d;
	else
		*xy += d;
	commit_key_move(c);
}
static void handle_key_move(struct JBWMClient * restrict c,
	const KeySym k, const bool mod)
{
	/* These operations invalid when fullscreen.  */
	if (c->opt.fullscreen)
		return;
	struct KeyMoveFlags f = {.mod = mod, .pos = true};
	switch (k) {
	case JBWM_KEY_LEFT:
		f.pos = false;
	case JBWM_KEY_RIGHT:
		f.horz = true;
		break;
	case JBWM_KEY_UP:
		f.pos = false;
	}
	key_move(c, f);
}
static void set_maximized(struct JBWMClient * restrict c)
{
	jbwm_set_horz(c);
	jbwm_set_vert(c);
}
static void set_not_maximized(struct JBWMClient * restrict c)
{
	jbwm_set_not_horz(c);
	jbwm_set_not_vert(c);
}
static void toggle_maximize(struct JBWMClient * restrict c)
{
	const struct JBWMClientOptions o = c->opt;
	// Honor !MWM_FUNC_MAXIMIZE
	// Maximizing shaped windows is buggy, so return.
	if (o.shaped || o.no_max || o.fullscreen)
		return;
	(o.max_horz && o.max_vert
	 ? set_not_maximized : set_maximized)(c);
}
__attribute__((nonnull))
static void handle_client_key_event(struct JBWMClient * restrict c,
	const bool mod, const KeySym key)
{
	Display * d = c->d;
	JBWM_LOG("handle_client_key_event: %d", (int)key);
	if (c->opt.fullscreen) {
		// only allow exiting from fullscreen
		if (key == JBWM_KEY_FS)
			jbwm_set_not_fullscreen(c);
		return; // prevent other operations while fullscreen
	}
	switch (key) {
	case JBWM_KEY_LEFT:
	case JBWM_KEY_RIGHT:
	case JBWM_KEY_UP:
	case JBWM_KEY_DOWN:
		handle_key_move(c, key, mod);
		break;
	case JBWM_KEY_KILL:
		jbwm_send_wm_delete(c);
		break;
	case JBWM_KEY_LOWER:
	case JBWM_KEY_ALTLOWER:
		XLowerWindow(d, c->parent);
		break;
	case JBWM_KEY_RAISE:
		XRaiseWindow(d, c->parent);
		break;
	case JBWM_KEY_FS:
		jbwm_set_fullscreen(c);
		break;
	case JBWM_KEY_MAX:
		toggle_maximize(c);
		break;
	case JBWM_KEY_MAX_H:
		(c->opt.max_horz ? jbwm_set_not_horz : jbwm_set_horz)(c);
		break;
	case JBWM_KEY_MAX_V:
		(c->opt.max_vert ? jbwm_set_not_vert : jbwm_set_vert)(c);
		break;
	case JBWM_KEY_STICK:
		jbwm_toggle_sticky(c);
		break;
	case JBWM_KEY_MOVE:
		jbwm_drag(c, false);
		break;
	case JBWM_KEY_SHADE:
		jbwm_toggle_shade(c);
		break;
	}
}
static struct JBWMClient * get_next_on_vdesk(void)
{
	struct JBWMClient *c = jbwm_get_current_client();
	struct JBWMScreen * restrict s = jbwm_get_screens();
	do {
		if (c) {
			c = c->next;
			if (!c && !jbwm_get_current_client())
				break;
		}
		if (!c) c = jbwm_get_head_client();
		if (!c || (c == jbwm_get_current_client()))
			break;
	} while (c->vdesk != s[c->screen].vdesk);
	return c;
}
static void next(void)
{
	struct JBWMClient * c = get_next_on_vdesk();
	if (!c)
		return;
	jbwm_restore_client(c);
	jbwm_select_client(c);
	point(c, 0, 0);
	point(c, c->size.width, c->size.height);
}
static void cond_client_to_desk(Display * restrict dpy,
	struct JBWMClient * c, struct JBWMScreen * s,
	const uint8_t d, const bool mod)
{
	mod && c ? jbwm_set_client_vdesk(c, d)
		: jbwm_set_vdesk(dpy, s, d);
}
static void start_terminal(void)
{
	if (fork() == 0) {
		char * t = getenv(JBWM_ENV_TERM);
		execlp(t, t, (char*)NULL);
	}
}
void jbwm_handle_key_event(Display * restrict d, XKeyEvent * restrict e)
{
	JBWM_LOG("jbwm_handle_key_event");
	const KeySym key = XLookupKeysym(e, 0);
	struct JBWMClient *c = jbwm_get_current_client();
	struct JBWMScreen *s = &jbwm_get_screens()[c ? c->screen : 0];
	struct {
		uint8_t vdesk:6;
		bool mod:1;
		bool zero:1;
	} opt = {s->vdesk, e->state & jbwm_keys_data.mod_mask, 0};
	switch (key) {
	case JBWM_KEY_NEW:
		start_terminal();
		break;
	case JBWM_KEY_QUIT:
		exit(0);
	case JBWM_KEY_NEXT:
		next();
		break;
	case XK_0:
		opt.zero = true;
	case XK_1: case XK_2: case XK_3: case XK_4: case XK_5:
	case XK_6: case XK_7: case XK_8: case XK_9:
		// First desktop 0, per wm-spec
		cond_client_to_desk(d, c, s, opt.zero
			? 10 : key - XK_1, opt.mod);
		break;
	case JBWM_KEY_PREVDESK:
		cond_client_to_desk(d, c, s, s->vdesk - 1, opt.mod);
		break;
	case JBWM_KEY_NEXTDESK:
		cond_client_to_desk(d, c, s, s->vdesk + 1, opt.mod);
		break;
	default:
		if (c)
			handle_client_key_event(c, opt.mod, key);
	}
}
__attribute__((nonnull))
static void grab(Display * restrict d, struct JBWMScreen * restrict s,
	KeySym * restrict ks, const uint32_t mask)
{
	for (; *ks; ++ks)
		XGrabKey(d, XKeysymToKeycode(d, *ks),
			 jbwm_keys_data.grab_mask | mask, s->root, true,
			 GrabModeAsync, GrabModeAsync);
}
void jbwm_grab_screen_keys(Display * restrict d,
	struct JBWMScreen * restrict s)
{
	grab(d, s, (KeySym[]){JBWM_KEYS_TO_GRAB}, 0);
	grab(d, s, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB},
		jbwm_keys_data.mod_mask);
}
