// Copyright 2020, Jeffrey E. Bedard
#ifndef JBWM_JBWMKEYS_H
#define JBWM_JBWMKEYS_H
#include <X11/keysym.h>
enum JBWMKeys {
    JBWM_KEY_ALTLOWER = XK_minus,
    JBWM_KEY_BOTTOMLEFT = XK_KP_End,
    JBWM_KEY_BOTTOMRIGHT = XK_KP_Page_Down,
    JBWM_KEY_DOWN = XK_j,
    JBWM_KEY_DOWN_UP = XK_J,
    JBWM_KEY_FS = XK_a,
    JBWM_KEY_INFO = XK_F5,
    JBWM_KEY_LEFT = XK_h,
    JBWM_KEY_LEFT_UP = XK_H,
    JBWM_KEY_LOWER = XK_Down,
    JBWM_KEY_MAX = XK_space,
    JBWM_KEY_MAX_H = XK_x,
    JBWM_KEY_MAX_V = XK_z,
    JBWM_KEY_MOVE = XK_m,
    JBWM_KEY_NEW = XK_Return,
    JBWM_KEY_NEXT = XK_Tab,
    JBWM_KEY_NEXTDESK = XK_Right,
    JBWM_KEY_PREVDESK = XK_Left,
    JBWM_KEY_QUIT = XK_Escape,
    JBWM_KEY_RAISE = XK_Up,
    JBWM_KEY_RIGHT = XK_l,
    JBWM_KEY_RIGHT_UP = XK_L,
    JBWM_KEY_SHADE = XK_s,
    JBWM_KEY_STICK = XK_period,
    JBWM_KEY_TOPLEFT = XK_KP_Home,
    JBWM_KEY_TOPRIGHT = XK_KP_Page_Up,
    JBWM_KEY_UP = XK_k,
    JBWM_KEY_UP_UP = XK_K,
};
#include "key_combos.h"
#endif//!JBWM_JBWMKEYS_H
