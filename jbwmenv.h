
#ifndef JBWM_ENVIRONMENT_H
#define JBWM_ENVIRONMENT_H
#include "titlebar.h"

typedef struct 
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
	bool need_cleanup;
	// Client tracking:
	Client *current;
	Client *head;
} JBWMEnvironment;

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
extern JBWMEnvironment jbwm;

#define D jbwm.X.dpy

#endif /* not JBWM_ENVIRONMENT_H */
