// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "keys.h"
#include <stdlib.h>
#include <unistd.h>
#include "JBWMScreen.h"
#include "client.h"
#include "config.h"
#include "drag.h"
#include "jbwm.h"
#include "key_masks.h"
#include "log.h"
#include "max.h"
#include "move_resize.h"
#include "screen.h"
#include "select.h"
#include "signal.h"
#include "snap.h"
#include "title_bar.h"
#include "util.h"
#include "wm_state.h"
//#define DEBUG_KEYS
#ifndef DEBUG_KEYS
#undef JBWM_LOG
#define JBWM_LOG(...)
#endif//!DEBUG_KEYS
void jbwm_grab_window_keys(Display * d, const Window win)
{
	jbwm_grab_button(d, win, jbwm_get_grab_mask(), AnyButton);
}
__attribute__((nonnull))
static void grab(Display * d, struct JBWMScreen * s,
	KeySym * ks, const uint32_t mask)
{
	const uint16_t m = jbwm_get_grab_mask();
	for (; *ks; ++ks)
		XGrabKey(d, XKeysymToKeycode(d, *ks), m | mask, s->root,
			true, GrabModeAsync, GrabModeAsync);
}
void jbwm_grab_screen_keys(Display * d, struct JBWMScreen * s)
{
	grab(d, s, (KeySym[]){JBWM_KEYS_TO_GRAB}, 0);
	grab(d, s, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB},
		jbwm_get_mod_mask());
}
