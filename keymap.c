/*
 *       Filename:  keymap.c
 *    Description:  Key combo handling routines
 *        Created:  11/18/2011 12:46:13 PM
 *       Compiler:  gcc
 *         Author:  Jeffrey E. Bedard
 *
 */

#include "jbwm.h"

static void
grabkey(Window w, KeyCode kc, const unsigned int mask)
{
	XGrabKey(jbwm.X.dpy, kc, mask, w, True, GrabModeAsync,
		GrabModeAsync);
	XGrabKey(jbwm.X.dpy, kc, mask | LockMask, w, True,
		GrabModeAsync, GrabModeAsync);
	/* Mod2Mask is NumLock */
	XGrabKey(jbwm.X.dpy, kc, mask | Mod2Mask, w, True,
		GrabModeAsync, GrabModeAsync);
}

static void
grab_keysym(Window w, const unsigned int mask, KeySym keysym)
{
	KeyCode kc;

	kc = XKeysymToKeycode(jbwm.X.dpy, keysym);
	grabkey(w, kc, mask);
	if(jbwm.keymasks.numlock)
		grabkey(w, kc, mask | jbwm.keymasks.numlock);
}

static void
grab_keys_for_mask(ScreenInfo * s, KeySym * ks,
	const unsigned int mask)
{
	for(; *ks; ks++)
		grab_keysym(s->root, jbwm.keymasks.grab | mask, *ks);
}

static void
grab_mod_keys_for_screen(ScreenInfo * s)
{
	KeySym mod_keys[] = JBWM_ALT_KEYS_TO_GRAB;

	grab_keys_for_mask(s, mod_keys, jbwm.keymasks.mod);
}

void
grab_keys_for_screen(ScreenInfo * s)
{
	KeySym keys[] = JBWM_KEYS_TO_GRAB;

	grab_keys_for_mask(s, keys, 0);
	grab_mod_keys_for_screen(s);
}
