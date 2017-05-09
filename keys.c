// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "keys.h"
#include "JBWMKeys.h"
#include "JBWMScreen.h"
#include "key_masks.h"
#include "util.h"
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
			True, GrabModeAsync, GrabModeAsync);
}
void jbwm_grab_screen_keys(Display * d, struct JBWMScreen * s)
{
	grab(d, s, (KeySym[]){JBWM_KEYS_TO_GRAB}, 0);
	grab(d, s, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB},
		jbwm_get_mod_mask());
}
