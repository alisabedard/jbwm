
#ifndef ARWM_CLIENT_STRUCT_H
#define ARWM_CLIENT_STRUCT_H

/* client structure */

typedef struct Client Client;
struct Client
{
	Window window;
	Window parent;
	ScreenInfo *screen;
	Colormap cmap;
	int ignore_unmap;

	XRectangle geometry;
	int border;
	XRectangle old_geometry;	/* used when maximising */

	unsigned int exposed_width;

	int min_width, min_height;
	int max_width, max_height;
	int width_inc, height_inc;
	int base_width, base_height;
	int win_gravity;
	int old_border;
	int vdesk;
	int shade_height;

	Atom window_type;

#define AR_CLIENT_IS_STICKY 		(1<<0)
/* Set when client needs to be removed: */ 
#define AR_CLIENT_REMOVE 		(1<<1)
#define AR_CLIENT_ACTIVE 		(1<<2)
#define AR_CLIENT_SHADED 		(1<<3)
#define AR_CLIENT_DONT_MANAGE 		(1<<4)
/* This flag is isolated to ensure code correctness.  */
#define AR_CLIENT_DONT_USE_TITLEBAR 	(1<<5)
#define AR_CLIENT_BLACK_BORDER 		(1<<6)
/* This flag is isolated to ensure code correctness.  */
#define AR_CLIENT_SHAPED 		(1<<7)

	unsigned int flags;

	/*
	 * This is stored within the structure to allow for one-time
	 * assignment and, thus, faster send_config calls.
	 */
	XConfigureEvent ce;
	Client *next;

#ifdef TITLEBAR
	Window info_window;
	Window close_button;
	Window resize_handle_right;
#endif	/* TITLEBAR */
};

/* This is to initialize the above CE.  */
void
     initialize_client_ce(Client * c);

#endif /* ARWM_CLIENT_STRUCT_H */
