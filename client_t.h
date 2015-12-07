
#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H

/* client structure */

typedef struct Client Client;
struct Client
{
	Window window;
	Window parent;
	ScreenInfo *screen;
#ifdef USE_CMAP
	Colormap cmap;
#endif				/* USE_CMAP */
	int ignore_unmap;

	XRectangle geometry;
	ubyte border;
	XRectangle old_geometry;	/* used when maximising */

	unsigned int exposed_width;

	XSizeHints size;

	byte vdesk;
	int shade_height;

	Atom window_type;

#define JB_CLIENT_IS_STICKY 		1
/* Set when client needs to be removed: */
#define JB_CLIENT_REMOVE		(1<<1)
#define JB_CLIENT_ACTIVE 		(1<<2)
#define JB_CLIENT_SHADED 		(1<<3)
#define JB_CLIENT_DONT_MANAGE 		(1<<4)
#define JB_CLIENT_NO_TB 		(1<<5)
#define JB_CLIENT_MAXIMIZED		(1<<6)

	ubyte flags;

	/*
	 * This is stored within the structure to allow for one-time
	 * assignment and, thus, faster send_config calls.
	 */
	XConfigureEvent ce;
	Client *next;

#ifdef USE_TBAR
	Window titlebar;
#endif
	Window close_button;
	Window resize_handle_right;
};

/* This is to initialize the above CE.  */
void initialize_client_ce(Client * c);

#define is_sticky(c) (c->flags & JB_CLIENT_IS_STICKY)
#define add_sticky(c) c->flags |= JB_CLIENT_IS_STICKY
#define remove_sticky(c) c->flags &= ~JB_CLIENT_IS_STICKY
#define toggle_sticky(c) c->flags ^= JB_CLIENT_IS_STICKY

#endif /* JBWM_CLIENT_STRUCT_H */
