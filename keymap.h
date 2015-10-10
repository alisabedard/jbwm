#ifndef _KEYMAP_H
#define _KEYMAP_H

#define KEY_NEXT	XK_Tab
#ifndef KEY_NEW
#define KEY_NEW		XK_Return
#endif /* !KEY_NEW */
#define KEY_LOCK	XK_F1
#define KEY_BROWSER	XK_F2
#define KEY_SLEEP	XK_F3
#define KEY_QUIT	XK_Escape
#define KEY_TOPLEFT	XK_KP_Home
#define KEY_TOPRIGHT	XK_KP_Page_Up
#define KEY_BOTTOMLEFT	XK_KP_End
#define KEY_BOTTOMRIGHT	XK_KP_Page_Down

#define KEY_RAISE	XK_Up
#define KEY_LOWER	XK_Down
#define KEY_PREVDESK	XK_Left
#define KEY_NEXTDESK	XK_Right

#ifdef AVOID_ALPHA
#define KEY_UP		XK_Up
#define KEY_DOWN	XK_Down
#define KEY_LEFT	XK_Left
#define KEY_RIGHT	XK_Right
#else /* AVOID_ALPHA */
#define KEY_UP		XK_k
#define KEY_DOWN	XK_j
#define KEY_LEFT	XK_h
#define KEY_RIGHT	XK_l
#endif

#define KEY_ALTLOWER	XK_minus
#define KEY_INFO	XK_F5
#define KEY_MAX		XK_space
#define KEY_FIX		XK_f
#define KEY_MOVE	XK_m
/*
 * Mixtures of Ctrl, Alt an Escape are used for things like VMWare and
 * XFree86/Cygwin, so the KILL key is an option in the Makefile
 */
#ifndef KEY_KILL
#define KEY_KILL	XK_F4
#endif

#define JBWM_VWM_KEYS \
		KEY_FIX, KEY_PREVDESK, KEY_NEXTDESK,\
		XK_1, XK_2, XK_3, XK_4, XK_5, XK_6, XK_7, XK_8, XK_9

#define JBWM_KEYS_TO_GRAB \
{\
	KEY_NEXT, KEY_NEW, KEY_LOCK, KEY_QUIT, KEY_KILL,\
	KEY_TOPLEFT, KEY_TOPRIGHT, KEY_BOTTOMLEFT, KEY_BOTTOMRIGHT,\
	KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP,\
	KEY_LOWER, KEY_ALTLOWER, KEY_INFO, KEY_MAX,\
	JBWM_VWM_KEYS, KEY_MOVE, KEY_RAISE, KEY_BROWSER, 0\
};

#define JBWM_ALT_KEYS_TO_GRAB \
{\
	KEY_KILL, KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP, 0\
};

void grab_keys_for_screen(ScreenInfo * s);

#endif /* _KEYMAP_H */
