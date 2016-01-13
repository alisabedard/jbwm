// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H

#include <stdint.h>

#define JB_STICKY 		1
#define JB_REMOVE		(1<<1)
#define JB_ACTIVE 		(1<<2)
#define JB_SHADED 		(1<<3)
#define JB_DONT_MANAGE 		(1<<4)
#define JB_NO_TB 		(1<<5)
#define JB_MAXIMIZED		(1<<6)
#define JB_NO_MIN		(1<<7)
#define JB_NO_RESIZE		(1<<8)
#define JB_TEAROFF		(1<<9)
#define JB_NO_BORDER		(1<<10)
#define JB_NO_CLOSE		(1<<11)
#define JB_NO_MAX		(1<<12)
#define JB_NO_MOVE		(1<<13)
#define JB_MODAL		(1<<14)
#define JB_SHAPED		(1<<15)
#define JB_NO_RESIZE_DECOR	(1<<16)
#define JB_NO_CLOSE_DECOR	(1<<17)
#define JB_NO_MIN_DECOR		(1<<18)
#define JB_NO_MAX_DECOR		(1<<19)

typedef struct Client Client; 
struct Client
{
	Client *next;
	Window window;
	Window parent;
	ScreenInfo *screen;
	XSizeHints size, old_size;
	ubyte vdesk;
	int ignore_unmap;
	ubyte border;
	uint32_t flags;
	int win_gravity_hint;

	// Optional fields:
#ifdef USE_CMAP
	Colormap cmap;
#endif//USE_CMAP
#ifdef USE_TBAR
	Window titlebar;
	int exposed_width;
	unsigned short shade_height;
#endif//USE_TBAR
};

/* This is to initialize the above CE.  */
void initialize_client_ce(Client * c);

#define is_sticky(c) (c->flags & JB_STICKY)
#define add_sticky(c) c->flags |= JB_STICKY
#define remove_sticky(c) c->flags &= ~JB_STICKY
#define toggle_sticky(c) c->flags ^= JB_STICKY

#endif /* JBWM_CLIENT_STRUCT_H */
