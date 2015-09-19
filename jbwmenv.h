
#ifndef ARWM_ENVIRONMENT_H
#define ARWM_ENVIRONMENT_H
#include "titlebar.h"

#ifdef USE_EWMH
typedef struct _ARWMNetWMWindowType
{
	Atom desktop, dock, toolbar, menu, utility, splash, dialog,
		dropdown_menu, popup_menu, tooltip, notification,
		combo, dnd, normal;
} ARWMNetWMWindowType;
#endif

struct ARWMEnvironment
{
	volatile Window initialising;
#ifdef USE_EWMH
	ARWMNetWMWindowType netwm;
#endif /* USE_EWMH */
	struct
	{
		unsigned int numlock, grab, mod;
	} keymasks;
	struct
	{
		/* Commonly used X information */
		Display *dpy;
#ifdef USE_XFT
		XftFont *font;
#else				/* ! USE_XFT */
		XFontStruct *font;
#endif				/* USE_XFT */
		ubyte num_screens;
		ScreenInfo *screens;
		Cursor cursor;
#ifdef USE_SHAPE
		int shape_event;
#endif /* USE_SHAPE */
	} X;
	ARWMTitlebarData titlebar;
#define ARWM_FLAG_NEED_TIDY 1
#define ARWM_FLAG_HAVE_SHAPE 1 << 1
	ubyte flags;
};

typedef struct ARWMEnvironment ARWMEnvironment;

typedef struct
{
	char *font;
	struct
	{
		char *fg;
		char *bg;
		char *fc;
	} color;
	char *term;
} GlobalOptions;

extern GlobalOptions opt;

#endif /* not ARWM_ENVIRONMENT_H */
