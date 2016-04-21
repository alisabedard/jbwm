// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H

#include <X11/Xutil.h>
#include "ScreenInfo.h"

#define JB_STICKY               1
#define JB_REMOVE               (1<<1)
#define JB_ACTIVE               (1<<2)
#define JB_SHADED               (1<<3)
#define JB_DONT_MANAGE          (1<<4)
#define JB_NO_TB                (1<<5)
#define JB_MAXIMIZED            (1<<6)
#define JB_NO_MIN               (1<<7)
#define JB_NO_RESIZE            (1<<8)
#define JB_TEAROFF              (1<<9)
#define JB_NO_BORDER            (1<<10)
#define JB_NO_CLOSE             (1<<11)
#define JB_NO_MAX               (1<<12)
#define JB_NO_MOVE              (1<<13)
#define JB_MODAL                (1<<14)
#define JB_SHAPED               (1<<15)
#define JB_NO_RESIZE_DECOR      (1<<16)
#define JB_NO_CLOSE_DECOR       (1<<17)
#define JB_NO_MIN_DECOR         (1<<18)
#define JB_NO_MAX_DECOR         (1<<19)
#define JB_MAX_HORZ		(1<<20)
#define JB_MAX_VERT		(1<<21)
#define JB_FULLSCREEN		(1<<22)
#define JB_IS_FS		(1<<23)

typedef struct Client Client;
struct Client {
	uint8_t vdesk;
	uint8_t ignore_unmap;
	uint8_t border;
#ifdef USE_TBAR
	uint16_t exposed_width;
	uint16_t shade_height;
#endif				//USE_TBAR
	uint32_t flags;
	Colormap cmap;
	Window window, parent;
#ifdef USE_TBAR
	Window titlebar;
#endif				//USE_TBAR
	XRectangle old_size;
	ScreenInfo *screen;
	Client *next;
	XSizeHints size;
};

/* This is to initialize the above CE.  */
void initialize_client_ce(Client * c);

#endif /* JBWM_CLIENT_STRUCT_H */
