// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef JBWM_JBWMDEFAULTS_H
#define JBWM_JBWMDEFAULTS_H
#include <X11/X.h>
/* Compile-time defaults of an integer type shall be stored here.  */
enum JBWMDefaults {
	JBWM_KEYMASK_GRAB = Mod4Mask,
	JBWM_KEYMASK_MOD = ControlMask,
	JBWM_SNAP = 16,
	JBWM_RESIZE_INCREMENT = 32,
	JBWM_MAX_DESKTOPS = 10
};
#endif//!JBWM_JBWMDEFAULTS_H
