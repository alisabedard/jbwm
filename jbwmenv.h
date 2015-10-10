
#ifndef JBWM_ENVIRONMENT_H
#define JBWM_ENVIRONMENT_H
#include "titlebar.h"

#ifdef USE_EWMH
typedef struct _JBWMNetWMWindowType
{
	Atom desktop, dock, toolbar, menu, utility, splash, dialog,
		dropdown_menu, popup_menu, tooltip, notification,
		combo, dnd, normal;
} JBWMNetWMWindowType;
#endif

struct JBWMEnvironment
{
	volatile Window initialising;
#ifdef USE_EWMH
	JBWMNetWMWindowType netwm;
#endif				/* USE_EWMH */
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
		ubyte have_shape;
		int shape_event;
#endif				/* USE_SHAPE */
	} X;
	JBWMTitlebarData titlebar;
	ubyte need_cleanup;
};

typedef struct JBWMEnvironment JBWMEnvironment;

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

#endif /* not JBWM_ENVIRONMENT_H */
