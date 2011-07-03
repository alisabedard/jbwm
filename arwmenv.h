
#ifndef ARWM_ENVIRONMENT_H
#define ARWM_ENVIRONMENT_H
#ifdef TITLEBAR
#include "titlebar.h"
#endif /* TITLEBAR */

typedef struct
_ARWMNetWMWindowType
{
	Atom desktop, dock, toolbar, menu, utility, splash,
		dialog, dropdown_menu, popup_menu, tooltip,
		notification, combo, dnd, normal;
} ARWMNetWMWindowType;

struct 
ARWMEnvironment
{
	volatile Window initialising;

	struct
	{
		struct
		{
			/* Standard X protocol atoms */
			Atom state, protos, delete, cmapwins;
		} wm;
		struct
		{
			Atom desktop, state, sticky, fullscreen;
		} vwm;
		Atom mwm_hints;
		ARWMNetWMWindowType net_wm_window_type;
	} atoms;
	struct
	{
		unsigned int numlock, grab1, grab2, alt;
	} keymasks;
	struct
	{
		/* Commonly used X information */
		Display *dpy;
		XftFont * font;
		int num_screens;
		ScreenInfo *screens;

		struct
		{
			Cursor move;
			Cursor resize;
		} cursors;
		int have_shape, shape_event;
	} X;
	struct
	{
		int bw;
	/*	char * terminal[3];
		const char * display=""; */
		int snap;
#ifdef SOLIDDRAG
		Bool solid_drag;
#endif /* SOLIDDRAG */
	} options;

#ifdef TITLEBAR
	ARWMTitlebarData titlebar;
#endif /* TITLEBAR */
};
typedef struct ARWMEnvironment ARWMEnvironment;
#endif /* not ARWM_ENVIRONMENT_H */
