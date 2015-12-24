/* Copyright 2007-2015, Jeffrey E. Bedard */
/*
 * This file is ment to hold configuration values and to resolve
 * configuration dependencies.
 */

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
#if 0
#define DEF_FONT "fixed"
#endif
#define DEF_FONT "-*-lucida-*-r-*-*-14-*-*-*-*-*-iso10646-*"
#endif /* USE_XFT */

/* Default colors */
#ifdef USE_COLORS
#define DEF_FG          "white"
#define DEF_FC          "dark green"
#define DEF_BG          "grey5"
#define TITLEBAR_CLOSE_BG	"salmon"
#define TITLEBAR_SHADE_BG	"light green"
#define TITLEBAR_RESIZE_BG	"cadet blue"
#else
#define DEF_FG          "white"
#define DEF_FC          "grey20"
#define DEF_BG          "grey10"
#define TITLEBAR_CLOSE_BG	"grey20"
#define TITLEBAR_SHADE_BG	"grey30"
#define TITLEBAR_RESIZE_BG	"grey40"
#endif /* USE_COLORS */

#define GRAB_MASK Mod4Mask
#define LOCK_MASK LockMask
#define MOD_MASK ControlMask

#define JBWM_BORDER 1
#define JBWM_SNAP 12
#define JBWM_RESIZE_INCREMENT 20

#endif /* JBWM_CONFIG_H */
