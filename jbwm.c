/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2015, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include <signal.h>
#include <X11/cursorfont.h>
#include <unistd.h>
#include <stdlib.h>
#include "jbwm.h"

/* Main application data structure.  */
JBWMEnvironment jbwm;

static void
initialize_JBWMEnvironment_keymasks(void)
{
	jbwm.keymasks.grab = GRAB_MASK;
	jbwm.keymasks.mod = MOD_MASK;
}

Application *head_app = NULL;

/* Client tracking information */
Client *head_client = NULL;
Client *current = NULL;

#ifdef USE_ARGV
static void
process_app_class_options(const char *name)
{
	char *tmp;
	Application *new = calloc(1, sizeof(Application));

	if((tmp = strchr(name, '/')))
	{
		*(tmp++) = 0;
		if(strlen(tmp) > 0)
		{
			new->res_class = malloc(strlen(tmp) + 1);
			strcpy(new->res_class, tmp);
		}
	}
	{
		const size_t namelen = strlen(name);

		if(namelen > 0)
		{
			new->res_name = malloc(namelen + 1);
			strcpy(new->res_name, name);
		}
	}
	new->next = head_app;
	head_app = new;
}

/* Used for overriding the default WM modifiers */
static unsigned int
parse_modifiers(char *s)
{
	static struct
	{
		const char *name;
		unsigned int mask;
	} modifiers[9] =
	{
		{
		"shift", ShiftMask},
		{
		"lock", LockMask},
		{
		"control", ControlMask},
		{
		"mod", Mod1Mask},
		{
		"mod1", Mod1Mask},
		{
		"mod2", Mod2Mask},
		{
		"mod3", Mod3Mask},
		{
		"mod4", Mod4Mask},
		{
		"mod5", Mod5Mask}
	};
	const char *delim = ",+";
	char *tmp;
	unsigned int ret = 0;
	for(tmp = strtok(s, delim); tmp; tmp = strtok(NULL, delim))
	{
		ubyte i = 0;

		while(i++ < 8)
		{
			const char * n=modifiers[i].name;

			if(!strncmp(n, tmp, sizeof(n)))
			{
				ret |= modifiers[i].mask;
			}
		}
	}

	return ret;
}

static void
parse_geometry(const char *arg)
{
	XRectangle g;

	if(!head_app)
		return;
	head_app->geometry_mask =
		XParseGeometry(arg, (int *)&g.x, (int *)&g.y,
		(unsigned int *)&g.width, (unsigned int *)&g.height);
	head_app->geometry = g;
}

static void
parse_argv(int argc, char **argv)
{
	char opt;
	const char *optstring="1:2:A:d:g:V";

	while((opt=getopt(argc, argv, optstring))!=-1)
	{
		switch(opt)
		{
			case '1':
				jbwm.keymasks.grab = parse_modifiers(optarg);
				break;
			case '2':
				jbwm.keymasks.mod = parse_modifiers(optarg);
				break;
			case 'A':
				process_app_class_options(optarg);
				break;
			case 'd':
				setenv("DISPLAY", optarg, 1);
				break;
			case 'g':
				parse_geometry(optarg);
				break;
			case 'V':
				puts(VERSION);
				exit(0);
			default: /* Usage */
				fprintf(stderr, "%s [%s]\n", argv[0],
					optstring);
				exit(1);
		}
	}
}
#endif /* USE_ARGV */

#ifdef USE_TBAR
static void
setup_fonts(void)
{
	Display *d;
	
	d = jbwm.X.dpy;
#ifdef USE_XFT
	{
		const ubyte s = DefaultScreen(d);

		jbwm.X.font=XftFontOpen(d, s, 
			XFT_FAMILY, XftTypeString, DEF_FONT,
			XFT_SIZE, XftTypeDouble, FONT_SIZE, NULL);
	}
#else
	jbwm.X.font=XLoadQueryFont(d, DEF_FONT);
#endif
	if(!jbwm.X.font)
		ERROR("bad font");
}
#endif /* USE_TBAR */

void
jbwm_grab_button(Window w, unsigned int mask, unsigned int btn)
{
	XGrabButton(jbwm.X.dpy, btn, mask, w, false, ButtonMask, 
		GrabModeAsync, GrabModeSync, None, None);
}

static void
setup_event_listeners(const ubyte i)
{
	XSetWindowAttributes attr;

	/* set up root window attributes 
	   - same for each screen */
	attr.event_mask =
		ChildMask | EnterWindowMask | ColormapChangeMask |
		PropertyChangeMask;

	XChangeWindowAttributes(jbwm.X.dpy, jbwm.X.screens[i].root,
		CWEventMask, &attr);
}

static inline void
allocate_colors(const ubyte i)
{
	jbwm.X.screens[i].fg=jbwm_color(DEF_FG);
	jbwm.X.screens[i].bg=jbwm_color(DEF_BG);
	jbwm.X.screens[i].fc=jbwm_color(DEF_FC);
}

static void
setup_each_client(const ubyte i, const ubyte j, Window *wins)
{
	XWindowAttributes winattr;

	XGetWindowAttributes(jbwm.X.dpy, wins[j], &winattr);
	if(!winattr.override_redirect
		&& winattr.map_state == IsViewable)
		make_new_client(wins[j], &jbwm.X.screens[i]);
}

static void
setup_clients(const ubyte i)
{
	Window dw1, dw2, *wins;
	unsigned int j, nwins;

	if(XQueryTree(jbwm.X.dpy, jbwm.X.screens[i].root, &dw1, &dw2,
		&wins, &nwins)==0)
		return;
	for(j = 0; j < nwins; j++)
		setup_each_client(i, j, wins);
	XFree(wins);
}

static void
setup_screen_elements(const ubyte i)
{
	jbwm.X.screens[i].screen = i;
	jbwm.X.screens[i].root = RootWindow(jbwm.X.dpy, i);
	jbwm.X.screens[i].vdesk = KEY_TO_VDESK(XK_1);
	jbwm.X.screens[i].width = DisplayWidth(jbwm.X.dpy, i);
	jbwm.X.screens[i].height = DisplayHeight(jbwm.X.dpy, i);
}

static void
setup_gc(const ubyte i)
{
	XGCValues gv;

	allocate_colors(i);
	gv.foreground = jbwm.X.screens[i].fg.pixel;
	gv.background = jbwm.X.screens[i].bg.pixel;
	/* set up GC parameters - same for each screen */
	gv.function = GXinvert;
	gv.subwindow_mode = IncludeInferiors;
	gv.line_width = JBWM_BORDER;
#ifdef USE_TBAR
#ifndef USE_XFT
	gv.font = jbwm.X.font->fid;
#endif /* ! USE_XFT */
#endif /* USE_TBAR */

	jbwm.X.screens[i].gc = XCreateGC(jbwm.X.dpy, jbwm.X.screens[i].root,
               GCFunction | GCSubwindowMode | GCLineWidth
#ifndef USE_XFT
               | GCFont
#endif /* ! USE_XFT */
               , &gv);
}

static void
setup_display_per_screen(const ubyte i)
{
	setup_screen_elements(i);
	setup_gc(i);
	setup_event_listeners(i);
	grab_keys_for_screen(&jbwm.X.screens[i]);
	/* scan all the windows on this screen */
	setup_clients(i);
}

#ifdef USE_SHAPE
static void
setup_shape(void)
{
	int __attribute__((unused)) e_dummy;

	jbwm.X.have_shape =
		XShapeQueryExtension(jbwm.X.dpy, &jbwm.X.shape_event,
		&e_dummy);
}
#endif /* USE_SHAPE */

static void
setup_screens(void)
{
	ubyte i;

	/* Now set up each screen in turn: jbwm.X.num_screens is used 
	   in scanning windows (XQueryTree) */
	i = jbwm.X.num_screens = ScreenCount(jbwm.X.dpy);
	jbwm.X.screens = malloc(i * sizeof(ScreenInfo));
	while(i--)
		setup_display_per_screen(i);
}

static void
setup_display(void)
{
	{
		const char *dpy_env = getenv("DISPLAY");

		if(!(jbwm.X.dpy = XOpenDisplay(dpy_env ? dpy_env : ":0")))
		{
			ERROR("bad DISPLAY");
		}
	}
	XSetErrorHandler(handle_xerror);
#ifdef USE_TBAR
	/* Fonts only needed with title bars */
	setup_fonts();
#endif /* USE_TBAR */
	jbwm.X.cursor = XCreateFontCursor(jbwm.X.dpy, XC_fleur);
#ifdef USE_SHAPE
	setup_shape();
#endif /* USE_SHAPE */
	setup_screens();
}

int 
main(
#ifdef USE_ARGV
	int argc, char **argv)
#else
	void)
#endif
{
	initialize_JBWMEnvironment_keymasks();
#ifdef USE_ARGV
	parse_argv(argc, argv);
#endif /* USE_ARGV */
	setup_display();
	main_event_loop();

	return 0;
}
