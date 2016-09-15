// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CONFIG_H
#define JBWM_CONFIG_H

#ifndef TERMINAL_CMD
#ifndef DEBIAN
#define TERMINAL_CMD "xterm&"
#else /* DEBIAN */
#define TERMINAL_CMD "x-terminal-emulator&"
#endif /* !DEBIAN */
#endif /* !TERMINAL_CMD */

/* Default fonts */
#ifdef USE_XFT
#define JBWM_DEF_FONT "Sans"
#define JBWM_FONT_SIZE 10.0
#else /* ! USE_XFT */
#define JBWM_DEF_FONT "-*-lucida-*-r-*-*-14-*-*-*-*-*-iso10646-*"
#define JBWM_FALLBACK_FONT "fixed"
#endif /* USE_XFT */

/* Default colors */
#define JBWM_DEF_FG "white"
#define JBWM_DEF_FC "green"
#define JBWM_DEF_BG "grey15"
#define JBWM_DEF_CLOSE "#977"
#define JBWM_DEF_SHADE "#797"
#define JBWM_DEF_RESIZE "#779"

// Environment
#define JBWM_ENV_DISPLAY "DISPLAY"
#define JBWM_ENV_FG "JBWM_FG"
#define JBWM_ENV_FC "JBWM_FC"
#define JBWM_ENV_BG "JBWM_BG"
#define JBWM_ENV_FONT "JBWM_FONT"

enum {
	GRAB_MASK = Mod4Mask,
	LOCK_MASK = LockMask,
	MOD_MASK = ControlMask
};

enum {
	JBWM_SNAP = 12,
	JBWM_RESIZE_INCREMENT = 20
};

#define JBWM_NAME "jbwm"

#endif//JBWM_CONFIG_H
