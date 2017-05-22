// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "keys.h"
#include "JBWMKeys.h"
#include "key_masks.h"
__attribute__((nonnull))
static void grab(Display * d, const Window root,
	KeySym * ks, const uint32_t modifiers)
{
	const uint16_t m = jbwm_get_grab_mask();
	for (; *ks; ++ks)
		XGrabKey(d, XKeysymToKeycode(d, *ks), m | modifiers, root,
			True, GrabModeAsync, GrabModeAsync);
}
void jbwm_grab_root_keys(Display * d, const Window root)
{
	grab(d, root, (KeySym[]){JBWM_KEYS_TO_GRAB}, 0);
	grab(d, root, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, jbwm_get_mod_mask());
}
