/* Copyright 2007, Jeffrey E. Bedard */
/*
 * This file is ment to hold configuration values and to resolve
 * configuration dependencies.
 */

#ifndef ARWM_CONFIG_H
#define ARWM_CONFIG_H

/* Default fonts */
#ifdef USE_XFT
#define DEF_FONT "Monospace 10"
#else /* ! USE_XFT */
#define DEF_FONT "9x15"
#endif /* USE_XFT */

/* Default colors */
#define DEF_FG          "#eef"
#define DEF_BG          "#444"
#define TITLEBAR_BG		0x40, 0x40, 0x40
#define TITLEBAR_RESIZE_BG	0x70, 0x70, 0xA0
#define TITLEBAR_CLOSE_BG	0xA0, 0x70, 0x80
#define TITLEBAR_SHADE_BG	0x70, 0xA0, 0x80

/* #define GRAB_MASK Mod1Mask */
#define GRAB_MASK Mod4Mask
#define LOCK_MASK LockMask
#define MOD_MASK ControlMask

#define ARWM_BORDER_WIDTH 1
#define ARWM_SNAP_DISTANCE 4

#define DEF_FC          "#ccf"
#define SPACE           3
#ifdef DEBIAN
#define DEF_TERM        "x-terminal-emulator"
#else
#define DEF_TERM        "urxvt"
#endif
#define XLOCK_CMD	"slock"
#define BROWSER_CMD	"firefox &"

#include "macros.h"

/* Sanity checks: */
#if defined(USE_XPM) && defined(USE_XBM)
#undef USE_XBM /* Prefer USE_XPM */
#endif /* USE_XPM && USE_XBM */

#define FONT_Y_OFFSET jbwm.X.font->descent
/* Values.  */
#ifdef USE_XFT
#define TITLE_FONT_HEIGHT jbwm.X.font->height
#else /* ! USE_XFT */
#define TITLE_FONT_HEIGHT (jbwm.X.font->ascent+jbwm.X.font->descent)
#endif /* USE_XFT */
/* Ensure that the title text fits within the titlebar.  */
#ifdef USE_TBAR
#define TITLEBAR_HEIGHT MAX(16,(TITLE_FONT_HEIGHT+ARWM_BORDER_WIDTH))
#else
#define TITLEBAR_HEIGHT 8
#endif

#define BUTTON_WIDTH (TITLEBAR_HEIGHT-ARWM_BORDER_WIDTH)
#define ARWM_RESIZE_INCREMENT TITLEBAR_HEIGHT

#define ARWM_DEFAULT_SNAP 8

#endif /* ARWM_CONFIG_H */
