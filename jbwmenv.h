// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_ENVIRONMENT_H
#define JBWM_ENVIRONMENT_H

typedef struct 
{
	struct
	{
		unsigned int grab, mod;
	} keymasks;
#ifdef USE_TBAR
	struct
	{
		bool tb_initialized;
#ifndef USE_XFT
		GC handle;
#endif//!USE_XFT
		GC close, shade, resize;
	} gc;
#endif//USE_TBAR
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
		uint8_t num_screens;
		ScreenInfo *screens;
		Cursor cursor;
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
