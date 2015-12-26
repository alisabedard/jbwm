
#ifndef JBWM_H
#define JBWM_H

#include <stdlib.h>
#include <stdbool.h>

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#ifdef USE_SHAPE
#include <X11/extensions/shape.h>
#endif /* USE_SHAPE */

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif

typedef char byte;
typedef unsigned char ubyte;

#include "ScreenInfo.h"
#include "client_t.h"
#include "jbwmenv.h"
#include "jbwmmod.h"

#ifdef MWM
/* Required for interpreting MWM hints: */
#define _XA_MWM_HINTS           "_MOTIF_WM_HINTS"
#define PROP_MWM_HINTS_ELEMENTS 3
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
typedef struct
{
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
} PropMwmHints;
#endif//MWM

void 
jbwm_grab_button(Window w, unsigned int mask, unsigned int button);

#ifdef EWMH
void
set_ewmh_allowed_actions(const Window w);
#endif//EWMH

#endif /* not JBWM_JBWM_H */
