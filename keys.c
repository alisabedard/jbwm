// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "keys.h"
//
#include "JBWMKeys.h"
#include "config.h"
//
#include <stdint.h>
//
__attribute__((nonnull(1,2)))
static void grab_r(Display * d, KeySym const * k,
  Window const r, uint32_t const mask)
{
  if (*k) { // Terminate at KeySym 0
    XGrabKey(d, XKeysymToKeycode(d, *k), mask, r, True,
      GrabModeAsync, GrabModeAsync);
    grab_r(d, k + 1, r, mask);
  }
}
void jbwm_grab_root_keys(Display * d, Window const root)
{
  grab_r(d, (KeySym[]){JBWM_KEYS_TO_GRAB}, root, JBWM_KEYMASK_GRAB);
  grab_r(d, (KeySym[]){JBWM_KEYS_TO_GRAB}, root, JBWM_KEYMASK_GRAB | LockMask);
  grab_r(d, (KeySym[]){JBWM_KEYS_TO_GRAB}, root, JBWM_KEYMASK_GRAB | Mod3Mask);
  grab_r(d, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, root, JBWM_KEYMASK_GRAB
    | JBWM_KEYMASK_MOD);
  grab_r(d, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, root, JBWM_KEYMASK_GRAB
    | JBWM_KEYMASK_MOD | LockMask);
  grab_r(d, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, root, JBWM_KEYMASK_GRAB
    | JBWM_KEYMASK_MOD | Mod3Mask);
}
