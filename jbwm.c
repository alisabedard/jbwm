/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2015, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include "jbwm.h"
#include <signal.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#ifdef USE_ARGV
#include <unistd.h>
#include <string.h>
#endif//USE_ARGV

/* Main application data structure.  */
JBWMEnvironment jbwm;

#ifdef USE_ARGV

/* Used for overriding the default WM modifiers */
static unsigned int
parse_modifiers(char *arg)
{
	LOG("parse_modifiers()");
	struct
	{
		const char *name;
		unsigned int mask;
	} m[] = { { "shift", ShiftMask}, { "lock", LockMask}, 
		{ "control", ControlMask}, { "mod", Mod1Mask}, 
		{ "mod1", Mod1Mask}, { "mod2", Mod2Mask},
		{ "mod3", Mod3Mask}, { "mod4", Mod4Mask},
		{ "mod5", Mod5Mask}};
	for(ubyte i=0; i<9; i++)
		if(!strcmp(m[i].name, arg))
			return m[i].mask;
	return 0;
}

static void
parse_argv(int argc, char **argv)
{
	LOG("parse_argv(%d,%s...)", argc, argv[0]);
	const char *optstring="1:2:d:V";

	for(int opt;(opt=getopt(argc, argv, optstring))!=-1;)
	{
		switch(opt)
		{
			case '1':
				jbwm.keymasks.grab = parse_modifiers(optarg);
				break;
			case '2':
				jbwm.keymasks.mod = parse_modifiers(optarg);
				break;
			case 'd':
				setenv("DISPLAY", optarg, 1);
				break;
#ifdef STDIO
			case 'V':
				puts(VERSION);
				exit(0);
			default: /* Usage */
				fprintf(stderr, "%s [%s]\n", argv[0],
					optstring);
#else//!STDIO
			default:
#endif//STDIO
				exit(1);
		}
	}
}
#endif//USE_ARGV

#ifdef USE_TBAR
static void
setup_fonts(void)
{
	Display *d = D;
#ifdef USE_XFT
	jbwm.X.font=XftFontOpen(d, DefaultScreen(d), XFT_FAMILY, 
		XftTypeString, DEF_FONT, XFT_SIZE, XftTypeDouble, 
		FONT_SIZE, NULL);
#else//!USE_XFT
	jbwm.X.font=XLoadQueryFont(d, DEF_FONT);
#endif//USE_XFT
	if(!jbwm.X.font) ERROR("bad font");
}
#endif//USE_TBAR 

void
jbwm_grab_button(Window w, unsigned int mask, unsigned int btn)
{
	XGrabButton(D, btn, mask, w, false, 
		ButtonPressMask|ButtonReleaseMask, GrabModeAsync, 
		GrabModeSync, None, None);
}

static void
setup_event_listeners(const ubyte i)
{
	XSetWindowAttributes attr={.event_mask=SubstructureRedirectMask
		|SubstructureNotifyMask|EnterWindowMask|PropertyChangeMask
#ifdef CMAP
		|ColormapChangeMask
#endif//CMAP
		};
	XChangeWindowAttributes(D, jbwm.X.screens[i].root,
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
	XGetWindowAttributes(D, wins[j], &winattr);
	if(!winattr.override_redirect && winattr.map_state == IsViewable)
		make_new_client(wins[j], &jbwm.X.screens[i]);
}

static void
setup_clients(const ubyte i)
{
	unsigned int nwins;
	Window *wins;
	Window d __attribute__((unused));
	if(XQueryTree(D, jbwm.X.screens[i].root, &d, &d, &wins, 
		&nwins)==0) return;
	for(unsigned int j = 0; j < nwins; j++) 
		setup_each_client(i, j, wins);
	XFree(wins);
}

static void
setup_screen_elements(const ubyte i)
{
	jbwm.X.screens[i].screen = i;
	jbwm.X.screens[i].root = RootWindow(D, i);
	jbwm.X.screens[i].vdesk = 0;
	jbwm.X.screens[i].width = DisplayWidth(D, i);
	jbwm.X.screens[i].height = DisplayHeight(D, i);
}

static void
setup_gc(const ubyte i)
{
	allocate_colors(i);
	XGCValues gv={.foreground=jbwm.X.screens[i].fg.pixel, 
		.background=jbwm.X.screens[i].bg.pixel, .function=GXinvert,
		.subwindow_mode=IncludeInferiors, .line_width=JBWM_BORDER,
#if defined(USE_TBAR) && !defined(USE_XFT)
		.font=jbwm.X.font->fid
#endif//USE_TBAR&&!USE_XFT
		};
	unsigned long vm=GCFunction|GCSubwindowMode|GCLineWidth|GCForeground
		|GCBackground;
#if defined(USE_TBAR) && !defined(USE_XFT)
	vm|=GCFont;
#endif//USE_TBAR&&!USE_XFT
	jbwm.X.screens[i].gc = XCreateGC(D, jbwm.X.screens[i].root,
               vm, &gv);
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
#ifdef EWMH
	setup_ewmh_for_screen(&jbwm.X.screens[i]);
#endif//EWMH
}

#ifdef USE_SHAPE
static void
setup_shape(void)
{
	int __attribute__((unused)) e_dummy;

	jbwm.X.have_shape =
		XShapeQueryExtension(D, &jbwm.X.shape_event,
		&e_dummy);
}
#endif /* USE_SHAPE */

static void
setup_screens(void)
{
	/* Now set up each screen in turn: jbwm.X.num_screens is used 
	   in scanning windows (XQueryTree) */
	ubyte i = jbwm.X.num_screens = ScreenCount(D);
	jbwm.X.screens = malloc(i * sizeof(ScreenInfo));
	while(i--) setup_display_per_screen(i);
}

#include <X11/Xproto.h>

int
handle_xerror(Display * dpy __attribute__((unused)), XErrorEvent * e)
{
        if((e->error_code==BadAccess) && (e->request_code 
                == X_ChangeWindowAttributes)) ERROR("RootWindowTaken");
        return 0; // Ignore everything else.         
}

static void
setup_display(void)
{
	if(!(D=XOpenDisplay(NULL)))
		ERROR("BadDISPLAY");
	XSetErrorHandler(handle_xerror);
#ifdef EWMH
	ewmh_init();
#endif//EWMH
#ifdef USE_TBAR
	/* Fonts only needed with title bars */
	setup_fonts();
#endif /* USE_TBAR */
	jbwm.X.cursor = XCreateFontCursor(D, XC_fleur);
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
	jbwm.keymasks.grab = GRAB_MASK;
	jbwm.keymasks.mod = MOD_MASK;
#ifdef USE_ARGV
	parse_argv(argc, argv);
#endif /* USE_ARGV */
	setup_display();
	main_event_loop();

	return 0;
}
