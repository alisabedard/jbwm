/* Copyright 2007, Jeffrey E. Bedard */
/*
 * This file is ment to hold configuration values and to resolve
 * configuration dependencies.
 */

#ifndef JBWM_CONFIG_H
#define JBWM_CONFIG_H

/* Default fonts */
#ifdef USE_XFT
#define DEF_FONT "Monospace 10"
#else /* ! USE_XFT */
#define DEF_FONT "9x15"
#endif /* USE_XFT */

/* Default colors */
#define DEF_FG          "#ffffff"
#define DEF_FC          "#00a000"
#define DEF_BG          "#202020"
#define TITLEBAR_CLOSE_BG	0x70, 0x60, 0x60
#define TITLEBAR_SHADE_BG	0x60, 0x70, 0x60
#define TITLEBJB_RESIZE_BG	0x60, 0x60, 0x70

/* #define GRAB_MASK Mod1Mask */
#define GRAB_MASK Mod4Mask
#define LOCK_MASK LockMask
#define MOD_MASK ControlMask

#define JBWM_BORDER_WIDTH 1
#define JBWM_SNAP_DISTANCE 4

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
#undef USE_XBM	/* Prefer USE_XPM */
#endif /* USE_XPM && USE_XBM */

#define FONT_Y_OFFSET jbwm.X.font->descent
/* Values.  */
#ifdef USE_XFT
#define TITLE_FONT_HEIGHT jbwm.X.font->height
#else /* ! USE_XFT */
#ifndef USE_XPM
#define TITLE_FONT_HEIGHT (jbwm.X.font->ascent+jbwm.X.font->descent)
#else /* USE_XPM */
#define TITLE_FONT_HEIGHT (jbwm.X.font->ascent+jbwm.X.font->descent+4)
#endif /* !USE_XPM */
#endif /* USE_XFT */

/* Ensure that the title text fits within the titlebar.  */
#ifdef USE_TBAR
#define TITLEBAR_HEIGHT MAX(16,(TITLE_FONT_HEIGHT+JBWM_BORDER_WIDTH))
#else
#define TITLEBAR_HEIGHT 8
#endif
#ifdef USE_XPM
#undef TITLEBAR_HEIGHT
#define TITLEBAR_HEIGHT 24
#endif

#define BUTTON_WIDTH (TITLEBAR_HEIGHT-JBWM_BORDER_WIDTH)
#define JBWM_RESIZE_INCREMENT TITLEBAR_HEIGHT

#define JBWM_DEFAULT_SNAP 8

#endif /* JBWM_CONFIG_H */
