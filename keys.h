// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef __KEYS_H
#define __KEYS_H

#include <X11/keysymdef.h>
#include <X11/Xlib.h>

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

#define jbwm_num_keys xk_0, xk_1, xk_2, xk_3, xk_4, xk_5, xk_6, xk_7,\
	xk_8, xk_9

#define jbwm_vwm_keys key_prevdesk, key_nextdesk, jbwm_num_keys

#define jbwm_direction_keys key_left, key_right, key_down, key_up

#define jbwm_keys_to_grab key_next, key_new, key_quit, key_kill,\
	jbwm_direction_keys, key_lower, key_altlower, key_max, key_stick,\
	jbwm_vwm_keys, key_move, key_raise, key_shade, key_max_h,\
	key_max_v, key_fs, 0

#define jbwm_alt_keys_to_grab key_kill, jbwm_direction_keys,\
	jbwm_vwm_keys, 0

void grab_keys_for_screen(ScreenInfo * restrict s);

#endif /* __KEYS_H */
