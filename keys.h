// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef __KEYS_H
#define __KEYS_H

#include "ScreenInfo.h"
#include <X11/keysym.h>

void jbwm_handle_key_event(XKeyEvent * restrict e);

enum { KEY_NEXT=XK_Tab, KEY_NEW=XK_Return, KEY_QUIT=XK_Escape,
	KEY_TOPLEFT=XK_KP_Home, KEY_TOPRIGHT=XK_KP_Page_Up,
	KEY_BOTTOMLEFT=XK_KP_End, KEY_BOTTOMRIGHT=XK_KP_Page_Down,
	KEY_RAISE=XK_Up, KEY_LOWER=XK_Down, KEY_PREVDESK=XK_Left,
	KEY_NEXTDESK=XK_Right, KEY_UP=XK_k, KEY_DOWN=XK_j, KEY_LEFT=XK_h,
	KEY_RIGHT=XK_l, KEY_MAX_H=XK_x, KEY_MAX_V=XK_z, KEY_ALTLOWER=XK_minus,
	KEY_INFO=XK_F5, KEY_MAX=XK_space, KEY_STICK=XK_period, KEY_MOVE=XK_m,
	KEY_SHADE=XK_s, KEY_FS=XK_a };
/*
 * Mixtures of Ctrl, Alt an Escape are used for things like VMWare and
 * XFree86/Cygwin, so the KILL key is an option in the Makefile
 */
#ifndef KEY_KILL
#define KEY_KILL        XK_q
#endif

#define JBWM_NUM_KEYS XK_0, XK_1, XK_2, XK_3, XK_4, XK_5, XK_6, XK_7,\
	XK_8, XK_9

#define JBWM_VWM_KEYS KEY_PREVDESK, KEY_NEXTDESK, JBWM_NUM_KEYS

#define JBWM_DIRECTION_KEYS KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP

#define JBWM_KEYS_TO_GRAB KEY_NEXT, KEY_NEW, KEY_QUIT, KEY_KILL,\
	JBWM_DIRECTION_KEYS, KEY_LOWER, KEY_ALTLOWER, KEY_MAX, KEY_STICK,\
	JBWM_VWM_KEYS, KEY_MOVE, KEY_RAISE, KEY_SHADE, KEY_MAX_H,\
	KEY_MAX_V, KEY_FS, 0

#define JBWM_ALT_KEYS_TO_GRAB KEY_KILL, JBWM_DIRECTION_KEYS,\
	JBWM_VWM_KEYS, 0

void grab_keys_for_screen(ScreenInfo * restrict s);

#endif /* __KEYS_H */
