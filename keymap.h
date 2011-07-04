#ifndef _KEYMAP_H
#define _KEYMAP_H

#define KEY_NEXT	XK_Tab
#define KEY_NEW		XK_Return
#define KEY_TOPLEFT	XK_y
#define KEY_TOPRIGHT	XK_u
#define KEY_BOTTOMLEFT	XK_b
#define KEY_BOTTOMRIGHT	XK_n
#define KEY_LEFT	XK_h
#define KEY_RIGHT	XK_l
#define KEY_DOWN	XK_j
#define KEY_UP		XK_k
#define KEY_LOWER	XK_Insert
#define KEY_ALTLOWER	XK_KP_Insert
#define KEY_INFO	XK_i
#define KEY_MAXVERT	XK_equal
#define KEY_MAX		XK_x
#define KEY_FIX		XK_f
#define KEY_PREVDESK	XK_Left
#define KEY_NEXTDESK	XK_Right
#define KEY_XLOCK	XK_z
#define KEY_QUIT	XK_q

/*
 * Mixtures of Ctrl, Alt an Escape are used for things like VMWare and
 * XFree86/Cygwin, so the KILL key is an option in the Makefile
 */
#ifndef KEY_KILL
#define KEY_KILL	XK_Escape
#endif


#define ARWM_VWM_KEYS \
                KEY_FIX, KEY_PREVDESK, KEY_NEXTDESK,\
                XK_1, XK_2, XK_3, XK_4, XK_5, XK_6, XK_7, XK_8

#define ARWM_KEYS_TO_GRAB \
{\
        KEY_NEW, KEY_KILL,\
        KEY_TOPLEFT, KEY_TOPRIGHT, KEY_BOTTOMLEFT, KEY_BOTTOMRIGHT,\
        KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP,\
        KEY_LOWER, KEY_ALTLOWER, KEY_INFO, KEY_MAXVERT, KEY_MAX,\
        ARWM_VWM_KEYS, KEY_XLOCK, 0\
};

#define ARWM_ALT_KEYS_TO_GRAB \
{\
        KEY_KILL, KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP, 0\
};

#endif /* _KEYMAP_H */
