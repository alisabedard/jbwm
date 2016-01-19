// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_ENVIRONMENT_H
#define JBWM_ENVIRONMENT_H

typedef struct 
{
#ifdef USE_TBAR
	struct
	{
		GC close, shade, resize;
		bool tb_initialized;
	} gc;
#endif//USE_TBAR
	struct
	{
		/* Commonly used X information */
		Display *dpy;
#ifdef USE_TBAR
#ifdef USE_XFT
		XftFont *font;
#else//! USE_XFT 
		XFontStruct *font;
#endif//USE_XFT
#endif//USE_TBAR 
		ScreenInfo *screens;
		Cursor cursor;
	} X;
	struct
	{
		unsigned int grab, mod;
	} keymasks;
	// Client tracking:
	Client *current;
	Client *head;
	bool need_cleanup;
} JBWMEnvironment; 

extern JBWMEnvironment jbwm;

// Convenience reference to display, ensure unique:
#ifdef D
#undef D
#endif//D
#define D jbwm.X.dpy

#endif /* not JBWM_ENVIRONMENT_H */
