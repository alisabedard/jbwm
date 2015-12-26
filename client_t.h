
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
	ubyte flags;

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

#define is_sticky(c) (c->flags & JB_CLIENT_IS_STICKY)
#define add_sticky(c) c->flags |= JB_CLIENT_IS_STICKY
#define remove_sticky(c) c->flags &= ~JB_CLIENT_IS_STICKY
#define toggle_sticky(c) c->flags ^= JB_CLIENT_IS_STICKY

#endif /* JBWM_CLIENT_STRUCT_H */
