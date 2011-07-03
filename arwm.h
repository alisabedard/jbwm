
#ifndef ARWM_H
#define ARWM_H

#define _GNU_SOURCE

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/shape.h>
#include <X11/Xft/Xft.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "macros.h"

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
}      PropMwmHints;

/* readability stuff */

#define KEY_TO_VDESK(key) ((key) - XK_1)
#define valid_vdesk(v) ((unsigned)(v) < KEY_TO_VDESK(XK_8))

#define RAISE           1
#define NO_RAISE        0	/* for unhide() */

#define MAXIMIZE_HORZ        (1<<0)
#define MAXIMIZE_VERT        (1<<1)

/* some coding shorthand */

#define ChildMask       (SubstructureRedirectMask|SubstructureNotifyMask)
#define ButtonMask      (ButtonPressMask|ButtonReleaseMask)
#define MouseMask       (ButtonMask|PointerMotionMask)

#define grab_pointer(w, mask, curs) \
	(XGrabPointer(arwm.X.dpy, w, False, mask, GrabModeAsync, GrabModeAsync, \
	None, curs, CurrentTime) == GrabSuccess)

void
arwm_grab_button(Window w, unsigned int mask, unsigned int button);

#define setmouse(w, x, y) XWarpPointer(arwm.X.dpy, None, w, 0, 0, 0, 0, x, y)
#define get_mouse_position(xp,yp,root) do { \
		Window dw; \
		int di; \
		unsigned int dui; \
		XQueryPointer(arwm.X.dpy, root, &dw, &dw, xp, yp, &di, &di, &dui); \
	} while (0)
#define gravitate(c) gravitate_client(c, 1)
#define ungravitate(c) gravitate_client(c, -1)

#define is_sticky(c) (c->flags & AR_CLIENT_IS_STICKY)
#define add_sticky(c) c->flags |= AR_CLIENT_IS_STICKY
#define remove_sticky(c) c->flags &= ~AR_CLIENT_IS_STICKY
#define toggle_sticky(c) c->flags ^= AR_CLIENT_IS_STICKY

#define discard_enter_events() do { \
		XEvent dummy; \
		XSync(arwm.X.dpy, False); \
		while (XCheckMaskEvent(arwm.X.dpy, EnterWindowMask, &dummy)); \
	} while (0)

/* screen structure */

typedef struct ScreenInfo ScreenInfo;
struct ScreenInfo
{
	int screen;
	Window root;
	GC invert_gc;
	XColor fg, bg;
	int vdesk;
	XColor fc;
	char *display;
};

#include "client_t.h"



typedef struct Application Application;
struct Application
{
	char *res_name;
	char *res_class;
	int geometry_mask;
	XRectangle geometry;
	int vdesk;
	Bool is_sticky;
	Application *next;
};

/* Declarations for global variables in main.c */

#include "arwmenv.h"

/* Main environment structure, to replace evilwm's
   large collection of global variables.  */
extern ARWMEnvironment arwm;

/* Things that affect user interaction */
extern const char *opt_term[3];
extern Application *head_app;

/* These will take some time to integrate into arwm structure.  */
/* Client tracking information */
extern Client *head_client;
extern Client *current;

/* client.c */

Client *find_client(Window w);
void gravitate_client(Client * c, int sign);
void select_client(Client * c);

void fix_client(Client * c);

void remove_client(Client * c);
void send_config(Client * c);
void set_wm_state(Client * c, int state);
void set_shape(Client * c);

/* misc.c */

extern int need_client_tidy;
extern int ignore_xerror;
int handle_xerror(Display * dsply, XErrorEvent * e);
void spawn(const char *const cmd[]);
void handle_signal(int signo);

/* new.c */

void make_new_client(Window w, ScreenInfo * s);
long get_wm_normal_hints(Client * c);

/* screen.c */

void drag(Client * c);
void moveresize(Client * c);
void maximize(Client * c, int hv);
void show_info(Client * c, KeySym key);
void sweep(Client * c);
void unhide(Client * c, int raise_win);
void next(void);

void hide(Client * c);
void switch_vdesk(ScreenInfo * s, int v);

ScreenInfo *find_screen(Window root);
ScreenInfo *find_current_screen(void);
void grab_keys_for_screen(ScreenInfo * s);

#include "arwmmod.h"

#define GETATOM(atom) XInternAtom(arwm.X.dpy, atom, False)

#endif /* not ARWM_ARWM_H */

