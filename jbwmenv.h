
#ifndef JBWM_ENVIRONMENT_H
#define JBWM_ENVIRONMENT_H
#include "titlebar.h"

struct JBWMEnvironment
{
	struct
	{
		unsigned int grab, mod;
	} keymasks;
	struct
	{
		/* Commonly used X information */
		Display *dpy;
#ifdef USE_TBAR
#ifdef USE_XFT
		XftFont *font;
#else				/* ! USE_XFT */
		XFontStruct *font;
#endif				/* USE_XFT */
#endif /* USE_TBAR */
		ubyte num_screens;
		ScreenInfo *screens;
		Cursor cursor;
#ifdef USE_SHAPE
		ubyte have_shape;
		int shape_event;
#endif				/* USE_SHAPE */
	} X;
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
