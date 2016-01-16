// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_H
#define JBWM_H

// Standard Library:
#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>

// X11:
#include <X11/cursorfont.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#ifdef USE_SHAPE
#include <X11/extensions/shape.h>
#endif /* USE_SHAPE */

// Xft:
#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif

// JBWM:
#include "ScreenInfo.h"
#include "client_t.h"
#include "jbwmenv.h"
#include "config.h"
#include "keys.h"
#include "screen.h"
#include "client.h"
#include "new.h"
#include "events.h"
#include "button_event.h"
#include "atoms.h"
#include "screen.h"
#include "log.h"

// Optional modules:
#ifdef USE_TBAR
#include "titlebar.h"
#endif//USE_TBAR 

#ifdef EWMH
#include "ewmh.h"
#endif//EWMH

#ifdef USE_SNAP
#include "snap.h"
#endif//USE_SNAP

// jbwm.c:
void 
jbwm_grab_button(Window w, unsigned int mask, unsigned int button);

#endif /* not JBWM_JBWM_H */
