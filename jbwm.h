
#ifndef JBWM_H
#define JBWM_H

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#ifdef USE_SHAPE
#include <X11/extensions/shape.h>
#endif /* USE_SHAPE */

#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif

#include "macros.h"

typedef char byte;
typedef unsigned char ubyte;

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

/* readability stuff */

#define KEY_TO_VDESK(key) ((key) - XK_1)
#define valid_vdesk(v) ((unsigned)(v) < KEY_TO_VDESK(XK_8))

#define RAISE           1
#define NO_RAISE        0	/* for unhide() */

#define MAXIMISE_HORZ        (1<<0)
#define MAXIMISE_VERT        (1<<1)

/* some coding shorthand */

#define ChildMask       (SubstructureRedirectMask|SubstructureNotifyMask)
#define ButtonMask      (ButtonPressMask|ButtonReleaseMask)
#define MouseMask       (ButtonMask|PointerMotionMask)

void jbwm_grab_button(Window w, unsigned int mask,
	unsigned int button);

#include "ScreenInfo.h"
#include "client_t.h"
#include "application_t.h"
#include "jbwmenv.h"

/* Main environment structure, to replace evilwm's
   large collection of global variables.  */
extern JBWMEnvironment jbwm;

/* Things that affect user interaction */
extern const char *opt_term[3];
extern Application *head_app;

/* These will take some time to integrate into jbwm structure.  */
/* Client tracking information */
extern Client *head_client;
extern Client *current;

#include "misc.h"

/* new.c */

void make_new_client(Window w, ScreenInfo * s);

#include "screen.h"
#include "jbwmmod.h"
#include "atoms.h"

#endif /* not JBWM_JBWM_H */
