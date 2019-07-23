// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_CONFIG_H
#define JBWM_CONFIG_H
#include <X11/X.h> // for keymasks
/* Note:  Also, adjust the values in JBWMDefaults.h as necessary.  */
#define JBWM_DEF_TERM "xterm"
/* Default fonts */
#define JBWM_DEF_FONT "Sans:size=10:aspect=1.2"
/* Default colors */
#define JBWM_DEF_FG "#bad0ff"
#define JBWM_DEF_FC "#00ff2f"
#define JBWM_DEF_BG "#3b4352"
#define JBWM_DEF_CLOSE "#ff2600"
#define JBWM_DEF_RESIZE "#00a1ff"
#define JBWM_DEF_SHADE "#ffe200"
#define JBWM_DEF_STICK "#5bf662"
// Environment
#define JBWM_ENV_BG "JBWM_BG"
#define JBWM_ENV_DISPLAY "DISPLAY"
#define JBWM_ENV_FG "JBWM_FG"
#define JBWM_ENV_FC "JBWM_FC"
#define JBWM_ENV_CLOSE "JBWM_CLOSE"
#define JBWM_ENV_FONT "JBWM_FONT"
#define JBWM_ENV_RESIZE "JBWM_RESIZE"
#define JBWM_ENV_SHADE "JBWM_SHADE"
#define JBWM_ENV_STICK "JBWM_STICK"
#define JBWM_ENV_TERM "JBWM_TERM"
#define JBWM_NAME "jbwm"
/* Compile-time defaults of an integer type shall be stored here.  */
enum {
    JBWM_KEYMASK_GRAB = Mod4Mask,
    JBWM_KEYMASK_MOD = ControlMask,
    JBWM_SNAP = 15,
    JBWM_RESIZE_INCREMENT = 20,
    JBWM_MAX_DESKTOPS = 10
};
#endif//JBWM_CONFIG_H
