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
grab(ScreenInfo * s, KeySym * ks, const unsigned int mask)
{
	while(*(ks++))
	{
		const int gm=GrabModeAsync;
	
		XGrabKey(jbwm.X.dpy, XKeysymToKeycode(jbwm.X.dpy, *ks), 
			jbwm.keymasks.grab| mask, s->root, true, gm, gm); 
	}
}

void
grab_keys_for_screen(ScreenInfo * s)
{
	KeySym keys[] = JBWM_KEYS_TO_GRAB;
	KeySym mod_keys[] = JBWM_ALT_KEYS_TO_GRAB;
	
	grab(s, keys, 0);
	grab(s, mod_keys, jbwm.keymasks.mod);
}

