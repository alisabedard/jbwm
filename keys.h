// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_KEYS_H
#define JBWM_KEYS_H
#include "JBWMScreen.h"
#include <X11/keysym.h>
void jbwm_handle_key_event(Display * restrict d, XKeyEvent * restrict e)
	__attribute__((nonnull));
void jbwm_grab_screen_keys(struct JBWMScreen * restrict s)
	__attribute__((nonnull));
void jbwm_grab_window_keys(const jbwm_window_t win);
void jbwm_set_grab_mask(const uint16_t mask);
void jbwm_set_mod_mask(const uint16_t mask);
enum { JBWM_KEY_NEXT=XK_Tab, JBWM_KEY_NEW=XK_Return, JBWM_KEY_QUIT=XK_Escape,
	JBWM_KEY_TOPLEFT=XK_KP_Home, JBWM_KEY_TOPRIGHT=XK_KP_Page_Up,
	JBWM_KEY_BOTTOMLEFT=XK_KP_End, JBWM_KEY_BOTTOMRIGHT=XK_KP_Page_Down,
	JBWM_KEY_RAISE=XK_Up, JBWM_KEY_LOWER=XK_Down,
	JBWM_KEY_PREVDESK=XK_Left, JBWM_KEY_NEXTDESK=XK_Right,
	JBWM_KEY_UP=XK_k, JBWM_KEY_DOWN=XK_j, JBWM_KEY_LEFT=XK_h,
	JBWM_KEY_RIGHT=XK_l, JBWM_KEY_MAX_H=XK_x, JBWM_KEY_MAX_V=XK_z,
	JBWM_KEY_ALTLOWER=XK_minus, JBWM_KEY_INFO=XK_F5,
	JBWM_KEY_MAX=XK_space, JBWM_KEY_STICK=XK_period, JBWM_KEY_MOVE=XK_m,
	JBWM_KEY_SHADE=XK_s, JBWM_KEY_FS=XK_a };
/*
 * Mixtures of Ctrl, Alt an Escape are used for things like VMWare and
 * XFree86/Cygwin, so the KILL key is an option in the Makefile
 */
#ifndef JBWM_KEY_KILL
#define JBWM_KEY_KILL        XK_q
#endif
#define JBWM_NUM_KEYS XK_0, XK_1, XK_2, XK_3, XK_4, XK_5, XK_6, XK_7,\
	XK_8, XK_9
#define JBWM_VWM_KEYS JBWM_KEY_PREVDESK, JBWM_KEY_NEXTDESK, JBWM_NUM_KEYS
#define JBWM_DIRECTION_KEYS JBWM_KEY_LEFT, JBWM_KEY_RIGHT, JBWM_KEY_DOWN,\
	JBWM_KEY_UP
#define JBWM_KEYS_TO_GRAB JBWM_KEY_NEXT, JBWM_KEY_NEW, JBWM_KEY_QUIT,\
	JBWM_KEY_KILL, JBWM_DIRECTION_KEYS, JBWM_KEY_LOWER,\
	JBWM_KEY_ALTLOWER, JBWM_KEY_MAX, JBWM_KEY_STICK,\
	JBWM_VWM_KEYS, JBWM_KEY_MOVE, JBWM_KEY_RAISE, JBWM_KEY_SHADE,\
	JBWM_KEY_MAX_H, JBWM_KEY_MAX_V, JBWM_KEY_FS, 0
#define JBWM_ALT_KEYS_TO_GRAB JBWM_KEY_KILL, JBWM_DIRECTION_KEYS,\
	JBWM_VWM_KEYS, 0
#endif /* JBWM_KEYS_H */
