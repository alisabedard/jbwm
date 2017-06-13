// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "keys.h"
#include "JBWMKeys.h"
#include "key_masks.h"
__attribute__((nonnull(1,2)))
static void grab_r(Display * d, KeySym * restrict k, const Window r,
	const uint32_t mask)
{
	if (*k) { // Terminate at KeySym 0
		XGrabKey(d, XKeysymToKeycode(d, *k), mask, r, True,
			GrabModeAsync, GrabModeAsync);
		grab_r(d, ++k, r, mask);
	}
}
void jbwm_grab_root_keys(Display * d, const Window root)
{
	const uint32_t m = jbwm_get_grab_mask();
	grab_r(d, (KeySym[]){JBWM_KEYS_TO_GRAB}, root, m);
	grab_r(d, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, root,
		m | jbwm_get_mod_mask());
}
