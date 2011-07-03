/* Copyright 2007, Jeffrey E. Bedard */
/*
 * This file is ment to hold configuration values and to resolve
 * configuration dependencies.
 */

#ifndef ARWM_CONFIG_H
#define ARWM_CONFIG_H


/* These are the default settings.  */
#define DEF_FONT "Sans-12"

#define DEF_FG          "#778"
#define DEF_BG          "#334"

#define TITLEBAR_BG	0xb5, 0xb6, 0xbe
#define DEF_BW          1
#define DEF_FC          "#bbc"
#define SPACE           2
#ifdef DEBIAN
#define DEF_TERM        "x-terminal-emulator"
#else
#define DEF_TERM        "xterm"
#endif

#include "macros.h"

/* Values.  */
#define TITLEBUTTON_PADDING (arwm.options.bw)
#define TITLE_FONT_HEIGHT arwm.X.font->height
#define TITLE_FONT_Y (TITLE_FONT_HEIGHT - TITLEBUTTON_PADDING)
/* Ensure that the title text fits within the titlebar.  */
/*#define TITLEBAR_HEIGHT MAX(22,(TITLE_FONT_HEIGHT+TITLEBUTTON_PADDING))*/
#define TITLEBAR_HEIGHT (TITLE_FONT_HEIGHT+TITLEBUTTON_PADDING)

#define BUTTON_WIDTH TITLE_FONT_HEIGHT
#define ARWM_RESIZE_INCREMENT TITLEBAR_HEIGHT

#define ARWM_DEFAULT_SNAP 4

#endif	/* ARWM_CONFIG_H */

