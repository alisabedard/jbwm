
#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H

/* client structure */

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

#define JB_CLIENT_IS_STICKY 		1
#define JB_CLIENT_REMOVE		(1<<1)
#define JB_CLIENT_ACTIVE 		(1<<2)
#define JB_CLIENT_SHADED 		(1<<3)
#define JB_CLIENT_DONT_MANAGE 		(1<<4)
#define JB_CLIENT_NO_TB 		(1<<5)
#define JB_CLIENT_MAXIMIZED		(1<<6)
#define JB_CLIENT_NO_MIN		(1<<7)
#define JB_CLIENT_NO_RESIZE		(1<<8)
#define JB_CLIENT_TEAROFF		(1<<9)
#define JB_CLIENT_NO_BORDER		(1<<10)
#define JB_CLIENT_NO_CLOSE		(1<<11)
#define JB_CLIENT_NO_MAX		(1<<12)
#define JB_CLIENT_NO_MOVE		(1<<13)
#define JB_CLIENT_MODAL			(1<<14)
#define JB_CLIENT_SHAPED		(1<<15)
	unsigned short flags;

	// Optional fields:
#ifdef EWMH
	int win_gravity_hint;
#endif//EWMH
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

#define is_sticky(c) (c->flags & JB_CLIENT_IS_STICKY)
#define add_sticky(c) c->flags |= JB_CLIENT_IS_STICKY
#define remove_sticky(c) c->flags &= ~JB_CLIENT_IS_STICKY
#define toggle_sticky(c) c->flags ^= JB_CLIENT_IS_STICKY

#endif /* JBWM_CLIENT_STRUCT_H */
