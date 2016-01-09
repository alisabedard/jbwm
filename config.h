/* Copyright 2007-2016, Jeffrey E. Bedard */
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
#define DEF_FONT "-*-lucida-*-r-*-*-14-*-*-*-*-*-iso10646-*"
#define FALLBACK_FONT "fixed"
#endif /* USE_XFT */

/* Default colors */
#define DEF_FG          	"#eef"
#define DEF_FC          	"#686"
#define DEF_BG          	"#445"
#define TITLEBAR_BG		"grey10"
#define TITLEBAR_CLOSE_BG	"#755"
#define TITLEBAR_SHADE_BG	"#575"
#define TITLEBAR_RESIZE_BG	"#557"

#define GRAB_MASK Mod4Mask
#define LOCK_MASK LockMask
#define MOD_MASK ControlMask

#define JBWM_BORDER 1
#define JBWM_SNAP 12
#define JBWM_RESIZE_INCREMENT 20

#endif /* JBWM_CONFIG_H */
