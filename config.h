// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CONFIG_H
#define JBWM_CONFIG_H

#ifndef TERMINAL_CMD
#ifndef DEBIAN
#define TERMINAL_CMD "urxvt &"
#else /* DEBIAN */
#define TERMINAL_CMD "x-terminal-emulator &"
#endif /* !DEBIAN */
#endif /* !TERMINAL_CMD */

/* Default fonts */
#ifdef USE_XFT
#define DEF_FONT "Sans"
#define FONT_SIZE 10.0
#else /* ! USE_XFT */
#define DEF_FONT "-*-lucida-*-r-*-*-14-*-*-*-*-*-iso10646-*"
#define FALLBACK_FONT "fixed"
#endif /* USE_XFT */

/* Default colors */
#define DEF_FG                  "#eef"
#define DEF_FC                  "#7a7"
#define DEF_BG                  "#222"
#define TITLEBAR_CLOSE_BG       "#977"
#define TITLEBAR_SHADE_BG       "#797"
#define TITLEBAR_RESIZE_BG      "#779"

#define GRAB_MASK Mod4Mask
#define LOCK_MASK LockMask
#define MOD_MASK ControlMask

#define JBWM_BORDER 1
#define JBWM_SNAP 12
#define JBWM_RESIZE_INCREMENT 20

#define DESKTOPS 10

#endif /* JBWM_CONFIG_H */
