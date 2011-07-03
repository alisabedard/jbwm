/*
 * arwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <arwm@6809.org.uk> see README for license and other details.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "arwm.h"
#include "log.h"

#define MAXIMUM_PROPERTY_LENGTH 4096

static void init_geometry(Client * c);
static void reparent(Client * c);

#ifdef XDEBUG
static const char *map_state_string(int map_state);
static const char *gravity_string(int gravity);
static void debug_wm_normal_hints(XSizeHints * size);

#else
#define debug_wm_normal_hints(s)
#endif

static void
set_class_attribute(Client * c, Application * a, ScreenInfo * s)
{
	LOG_DEBUG("set_class_attribute(c,a,s);\n");
	if (a->geometry_mask & WidthValue)
		c->geometry.width = a->geometry.width * c->width_inc;
	if (a->geometry_mask & HeightValue)
		c->geometry.height = a->geometry.height * c->height_inc;
	if (a->geometry_mask & XValue)
		c->geometry.x=(a->geometry_mask & XNegative)
			? a->geometry.x + DisplayWidth(arwm.X.dpy, 
			s->screen) - c->geometry.width 
			- c->border : a->geometry.x + c->border;
	if (a->geometry_mask & YValue)
		c->geometry.y=(a->geometry_mask & YNegative)
			? a->geometry.y + DisplayHeight(arwm.X.dpy, 
			s->screen) - c->geometry.height
			- c->border : a->geometry.y + c->border;
	moveresize(c);
	if (a->vdesk != -1)
		c->vdesk = a->vdesk;
	if(a->is_sticky) 
		add_sticky(c);
	else
		remove_sticky(c);
}

static void
test_set_class_attribute(Client * c, Application * a, XClassHint * class,
	ScreenInfo * s)
{
	if ((!a->res_name || (class->res_name 
		&& !strcmp(class->res_name, a->res_name)))
		&& (!a->res_class || (class->res_class 
		&& !strcmp(class->res_class, a->res_class))))
		set_class_attribute(c, a, s);
}

static void
set_class_attributes(Client * c, Window w, ScreenInfo * s)
{
	XClassHint *class;

	/*
	 * Read instance/class information for client and check against list
	 * built with -app options
	 */
	if((class=XAllocClassHint()))
	{
		Application *a;

		XGetClassHint(arwm.X.dpy, w, class);
		for(a=head_app;a;a=a->next)
			test_set_class_attribute(c, a, class, s);
		XFree(class->res_name);
		XFree(class->res_class);
		XFree(class);
	}
}

static void
map_client(Client * c, ScreenInfo * s)
{
	/*
	 * Only map the window frame (and thus the window) if it's supposed
	 * to be visible on this virtual desktop.
	 */
	if (s->vdesk == c->vdesk)
		unhide(c, RAISE);
	else
		set_wm_state(c, IconicState);
	ARWM_UPDATE_NET_WM_DESKTOP(c);
}

static void
initialize_client_fields(Client * c, ScreenInfo * s, Window w)
{
	c->next = head_client;
	head_client = c;

	c->screen = s;
	c->window = w;
	c->ignore_unmap = 0;
#ifdef TITLEBAR
	c->info_window=0;
#endif /* TITLEBAR */
	c->flags=0;
	c->window_type=None;
}

void 
make_new_client(Window w, ScreenInfo * s)
{
	Client *c;
	char *name;

#ifdef DEBUG /* Validate inputs.  */
	assert(w);
	assert(s);
	assert(arwm.X.dpy);
#endif /* DEBUG */
	XGrabServer(arwm.X.dpy);

	/*
	 * First a bit of interaction with the error handler due to X's
	 * tendency to batch event notifications.  We set a global variable
	 * to the id of the window we're initialising then do simple X call
	 * on that window.  If an error is raised by this (and nothing else
	 * should do so as we've grabbed the server), the error handler
	 * resets the variable indicating the window has already disappeared,
	 * so we stop trying to manage it.
	 */
	arwm.initialising = w;
	XFetchName(arwm.X.dpy, w, &name);
	XSync(arwm.X.dpy, False);
	/*
	 * If 'initialising' is now set to None, that means doing the
	 * XFetchName raised BadWindow - the window has been removed before
	 * we got a chance to grab the server.
	 */
	if (arwm.initialising == None)
	{
		LOG_DEBUG("make_new_client() : XError occurred for initialising window - aborting...\n");
		XUngrabServer(arwm.X.dpy);
		return;
	}
	arwm.initialising = None;
	LOG_DEBUG("make_new_client(): %s\n", name ? name : "Untitled");
	if (name)
		XFree(name);

	c = malloc(sizeof(Client));
	/* Don't crash the window manager, just fail the operation. */
	if (!c)
	{
		LOG_ERROR("out of memory in new_client; limping onward\n");
		return;
	}
	initialize_client_fields(c, s, w);
	/*
	 * Ungrab the X server as soon as possible. Now that the client is
	 * malloc()ed and attached to the list, it is safe for any subsequent
	 * X calls to raise an X error and thus flag it for removal.
	 */
	XUngrabServer(arwm.X.dpy);

	c->border = arwm.options.bw;

	init_geometry(c);
	if(c->flags & AR_CLIENT_DONT_MANAGE)
		return;
#ifdef DEBUG
	{
		Client *p;
		int i = 0;

		for (p = head_client; p; p = p->next)
			i++;
		LOG_DEBUG("\nmake_new_client() : new window %dx%d+%d+%d,"
			" wincount=%d\n (%s:%d)\n", c->geometry.width, 
			c->geometry.height, c->geometry.x, c->geometry.y, i,
			__FILE__, __LINE__);
	}
#endif


	XSelectInput(arwm.X.dpy, c->window, EnterWindowMask 
		| PropertyChangeMask
#ifdef COLOURMAP
		| ColormapChangeMask
#endif	/* COLOURMAP */
		);
#ifdef SHAPE
	set_shape(c);
#endif
	reparent(c);
	set_class_attributes(c, w, s);
	map_client(c, s);
}

/*
 * Calls XGetWindowAttributes, XGetWMHints and XGetWMNormalHints to determine
 * window's initial geometry.
 * 
 * XGetWindowAttributes
 */


static unsigned long
handle_mwm_hints(Client * c)
{
	PropMwmHints * mprop;
	unsigned long nitems;
	const Atom mwm_hints=arwm.atoms.mwm_hints;

	if ((mprop = arwm_get_property(c->window, mwm_hints, 
		mwm_hints, &nitems)))
	{
		if (nitems >= PROP_MWM_HINTS_ELEMENTS
			&& (mprop->flags & MWM_HINTS_DECORATIONS)
			&& !(mprop->decorations & MWM_DECOR_ALL)
			&& !(mprop->decorations & MWM_DECOR_BORDER))
		{
			c->border = 0;
		}
		XFree(mprop);
	}

	arwm_handle_ewmh_hints(c);

	return nitems;
}

#ifdef DEBUG
void
print_size_hints(Window w)
{
	XSizeHints * sizes;

	sizes=XAllocSizeHints();
	XGetWMSizeHints(arwm.X.dpy, w, sizes, NULL, 
		XA_WM_SIZE_HINTS);
	printf("%s:%d:\twidth:  %d\theight:  %d\n", __FILE__,
		__LINE__, sizes->width, sizes->height);
	fflush(stdout);
	XFree(sizes);
}
#endif /* DEBUG */

static void 
init_geometry(Client * c)
{
	long size_flags;
	XWindowAttributes attr;
	unsigned long nitems;
	unsigned long i;
	unsigned long *lprop;
	Atom *aprop;

	initialize_client_ce(c);
	nitems=handle_mwm_hints(c);
	c->vdesk = c->screen->vdesk;
	if ((lprop = arwm_get_property(c->window, arwm.atoms.vwm.desktop, 
		XA_CARDINAL, &nitems)))
	{
		if (nitems && valid_vdesk(lprop[0]))
			c->vdesk = lprop[0];
		XFree(lprop);
	}
	remove_sticky(c);
	if ((aprop = arwm_get_property(c->window, arwm.atoms.vwm.state, 
		XA_ATOM, &nitems)))
	{
		for (i = 0; i < nitems; i++)
			if (aprop[i] == arwm.atoms.vwm.sticky)
				add_sticky(c);
		XFree(aprop);
	}

	/* Get current window attributes */
	LOG_XDEBUG("XGetWindowAttributes() @ %s:%d\n", 
		__FILE__, __LINE__);
	XGetWindowAttributes(arwm.X.dpy, c->window, &attr);
	LOG_XDEBUG("\t(%s) %dx%d+%d+%d, bw = %d\n", 
		map_state_string(attr.map_state), attr.width, 
		attr.height, attr.x, attr.y, attr.border_width);
	c->old_border = attr.border_width;
	c->old_geometry.width = c->old_geometry.height = 0;
#ifdef COLOURMAP
	c->cmap = attr.colormap;
#endif

	size_flags = get_wm_normal_hints(c);

	if ((attr.width >= c->min_width) && (attr.height >= c->min_height))
	{
		/*
		 * if (attr.map_state == IsViewable || (size_flags & (PSize |
		 * USSize))) {
		 */
		c->geometry.width = attr.width;
		c->geometry.height = attr.height;
	}
	else
	{
		c->geometry.width = c->min_width;
		c->geometry.height = c->min_height;
		send_config(c);
	}
	if ((attr.map_state == IsViewable)
		|| (size_flags & ( /* PPosition | */ USPosition)))
	{
		c->geometry.x = attr.x;
		c->geometry.y = attr.y;
	}
	else
	{
		int xmax = DisplayWidth(arwm.X.dpy, c->screen->screen);
		int ymax = DisplayHeight(arwm.X.dpy, c->screen->screen);
		int x, y;

		get_mouse_position(&x, &y, c->screen->root);
		c->geometry.x = (x * (xmax - c->border 
			- c->geometry.width)) / xmax;
		c->geometry.y = (y * (ymax - c->border 
			- c->geometry.height)) / ymax;
		send_config(c);
	}
	LOG_DEBUG("\twindow started as %dx%d +%d+%d\n", 
		c->geometry.width, c->geometry.height, 
		c->geometry.x, c->geometry.y);
	/* Test if the reparent that is to come 
		would trigger an unmap event. */
	if (attr.map_state == IsViewable)
		c->ignore_unmap++;
	gravitate(c);
}

static void 
reparent(Client * c)
{
	XSetWindowAttributes p_attr;

	p_attr.border_pixel = c->screen->bg.pixel;
	p_attr.override_redirect = True;
	p_attr.event_mask = ChildMask | ButtonPressMask | EnterWindowMask;

#ifdef SHAPE
	if(!(c->flags & AR_CLIENT_SHAPED))
#endif /* SHAPE */
		p_attr.background_pixel = c->screen->bg.pixel;

	c->parent
		= XCreateWindow(arwm.X.dpy, c->screen->root, 
		c->geometry.x - c->border,
		c->geometry.y - c->border
#ifdef TITLEBAR
		-( 
#ifdef SHAPE
		(c->flags & AR_CLIENT_SHAPED) ? 0 :
#endif
		TITLEBAR_HEIGHT)
#endif /* TITLEBAR */
		, c->geometry.width,
		c->geometry.height
#ifdef TITLEBAR
		+ (
#ifdef SHAPE
		(c->flags & AR_CLIENT_SHAPED) ? 0 :
#endif /* SHAPE */
		TITLEBAR_HEIGHT)
#endif	/* TITLEBAR */
		,
		c->border,
		DefaultDepth(arwm.X.dpy, c->screen->screen),
		CopyFromParent,
		DefaultVisual(arwm.X.dpy, c->screen->screen),
		CWOverrideRedirect | CWBorderPixel 
//#ifndef SHAPE
		| CWBackPixel
//#endif /* ! SHAPE */
		| CWEventMask, &p_attr);

	XAddToSaveSet(arwm.X.dpy, c->window);
	XSetWindowBorderWidth(arwm.X.dpy, c->window, 0);
#ifdef TITLEBAR
	if(!(c->flags & AR_CLIENT_DONT_USE_TITLEBAR))
		update_info_window(c);
#ifdef DEBUG
	assert(c);
	assert(c->window);
	assert(c->parent);
#endif /* DEBUG */
	LOG_DEBUG("reparent window:\n");
	if(!(c->flags & AR_CLIENT_DONT_MANAGE))
	{
		XReparentWindow(arwm.X.dpy, c->window, c->parent, 0, 
			(
#ifdef SHAPE
			(c->flags & AR_CLIENT_SHAPED) ? 0 :
#endif /* SHAPE */
			TITLEBAR_HEIGHT)); 
	}
#else	/* not TITLEBAR */
	XReparentWindow(arwm.X.dpy, c->window, c->parent, 0, 0);
#endif	/* TITLEBAR */
	LOG_DEBUG("map window:\n");
	XMapWindow(arwm.X.dpy, c->window);
	LOG_DEBUG("grab button:\n");
	arwm_grab_button(c->parent, arwm.keymasks.grab2, AnyButton);
	LOG_DEBUG("done reparenting\n");
	XUngrabPointer(arwm.X.dpy, CurrentTime);
	/*XGrabServer(arwm.X.dpy);
	moveresize(c);
	XUngrabServer(arwm.X.dpy);*/
/*	send_config(c); */
}

/* Get WM_NORMAL_HINTS property */
long 
get_wm_normal_hints(Client * c)
{
	XSizeHints *size;
	long flags;
	long dummy;

	size = XAllocSizeHints();
	LOG_XDEBUG("XGetWMNormalHints()\n");
	XGetWMNormalHints(arwm.X.dpy, c->window, size, &dummy);
	debug_wm_normal_hints(size);
	flags = size->flags;
#define SIZE_TYPE_ASSIGN(type)\
	c->type##_width=size->type##_width;\
	c->type##_height=size->type##_height;
	if (flags & PMinSize)
	{
		SIZE_TYPE_ASSIGN(min);
	}
	else
		c->min_width = c->min_height = 0;
	if (flags & PMaxSize)
	{
		SIZE_TYPE_ASSIGN(max);
	}
	else
		c->max_width = c->max_height = 0;
	if (flags & PBaseSize)
	{
		SIZE_TYPE_ASSIGN(base);
	}
	else
	{
		c->base_width = c->min_width;
		c->base_height = c->min_height;
	}
	c->width_inc = c->height_inc = 1;
	if (flags & PResizeInc)
	{
		c->width_inc = size->width_inc ? size->width_inc : 1;
		c->height_inc = size->height_inc ? size->height_inc : 1;
	}
	if (!(flags & PMinSize))
	{
		c->min_width = c->base_width + c->width_inc;
		c->min_height = c->base_height + c->height_inc;
	}
	c->win_gravity = (flags & PWinGravity) 
		? size->win_gravity : NorthWestGravity;
	XFree(size);
	return flags;
}

void *
arwm_get_property(Window w, Atom property, Atom req_type,
	unsigned long *nitems_return)
{
	Atom actual_type;
	int actual_format;
	unsigned long bytes_after;
	unsigned char *prop;

	if (XGetWindowProperty(arwm.X.dpy, w, property,
		0L, MAXIMUM_PROPERTY_LENGTH / 4, False,
		req_type, &actual_type, &actual_format,
		nitems_return, &bytes_after, &prop)
		== Success)
	{
		if (actual_type == req_type)
			return (void *)prop;
		else
			XFree(prop);
	}
	return NULL;
}

#ifdef XDEBUG
static const char *
map_state_string(int map_state)
{
	const char *map_states[4] = {
		"IsUnmapped",
		"IsUnviewable",
		"IsViewable",
		"Unknown"
	};

	return ((unsigned int)map_state < 3)
		? map_states[map_state]
		: map_states[3];
}

static const char *
gravity_string(int gravity)
{
	const char *gravities[12] = {
		"ForgetGravity",
		"NorthWestGravity",
		"NorthGravity",
		"NorthEastGravity",
		"WestGravity",
		"CenterGravity",
		"EastGravity",
		"SouthWestGravity",
		"SouthGravity",
		"SouthEastGravity",
		"StaticGravity",
		"Unknown"
	};

	return  gravities[((unsigned int)gravity < 11)?gravity:11];
}

static void 
debug_wm_normal_hints(XSizeHints * size)
{
	if (size->flags & 15)
	{
		LOG_XDEBUG("\t");
		if (size->flags & USPosition)
		{
			LOG_XDEBUG("USPosition ");
		}
		if (size->flags & USSize)
		{
			LOG_XDEBUG("USSize ");
		}
		if (size->flags & PPosition)
		{
			LOG_XDEBUG("PPosition ");
		}
		if (size->flags & PSize)
		{
			LOG_XDEBUG("PSize");
		}
		LOG_XDEBUG("\n");
	}
	if (size->flags & PMinSize)
	{
		LOG_XDEBUG("\tPMinSize: min_width = %d, min_height = %d\n", size->min_width, size->min_height);
	}
	if (size->flags & PMaxSize)
	{
		LOG_XDEBUG("\tPMaxSize: max_width = %d, max_height = %d\n", size->max_width, size->max_height);
	}
	if (size->flags & PResizeInc)
	{
		LOG_XDEBUG("\tPResizeInc: width_inc = %d, height_inc = %d\n",
			size->width_inc, size->height_inc);
	}
	if (size->flags & PAspect)
	{
		LOG_XDEBUG("\tPAspect: min_aspect = %d/%d, max_aspect = %d/%d\n",
			size->min_aspect.x, size->min_aspect.y,
			size->max_aspect.x, size->max_aspect.y);
	}
	if (size->flags & PBaseSize)
	{
		LOG_XDEBUG("\tPBaseSize: base_width = %d, base_height = %d\n",
			size->base_width, size->base_height);
	}
	if (size->flags & PWinGravity)
	{
		LOG_XDEBUG("\tPWinGravity: %s\n", gravity_string(size->win_gravity));
	}
}

#endif /* XDEBUG */
