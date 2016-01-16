// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

/* Main application data structure.  */
JBWMEnvironment jbwm;

/* Several following functions are declared cold as they are only used on
program startup.  */

#ifdef USE_ARGV
/* Used for overriding the default WM modifiers */
__attribute__((cold))
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
	for(uint8_t i=0; i<9; i++)
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
#ifdef USE_XFT
	jbwm.X.font=XftFontOpen(D, DefaultScreen(D), XFT_FAMILY, 
		XftTypeString, DEF_FONT, XFT_SIZE, XftTypeDouble, 
		FONT_SIZE, NULL);
#else//!USE_XFT
	jbwm.X.font=XLoadQueryFont(D, DEF_FONT);
	if(!jbwm.X.font)
		jbwm.X.font=XLoadQueryFont(D, FALLBACK_FONT);
#endif//USE_XFT
	if(!jbwm.X.font) ERROR("FONT");
}
#endif//USE_TBAR 

void
jbwm_grab_button(Window w, unsigned int mask, unsigned int btn)
{
	XGrabButton(D, btn, mask, w, false, 
		ButtonPressMask|ButtonReleaseMask, GrabModeAsync, 
		GrabModeSync, None, None);
}

__attribute__((cold))
static void
setup_event_listeners(const Window root)
{
	XSetWindowAttributes attr={.event_mask=SubstructureRedirectMask
		|SubstructureNotifyMask|EnterWindowMask|PropertyChangeMask};
#ifdef CMAP
	attr.event_mask|=ColormapChangeMask
#endif//CMAP
	XChangeWindowAttributes(D, root, CWEventMask, &attr);
}

__attribute__((cold))
static inline void
allocate_colors(ScreenInfo *s)
{
	XColor nullc; 
	const Colormap cm=DefaultColormap(D, s->screen);
	XAllocNamedColor(D, cm, DEF_FG, &s->fg, &nullc);
	XAllocNamedColor(D, cm, DEF_BG, &s->bg, &nullc);
	XAllocNamedColor(D, cm, DEF_FC, &s->fc, &nullc);
}

static inline void
setup_clients(ScreenInfo *s)
{
	unsigned int nwins;
	Window *wins;
	Window d;
	if(!XQueryTree(D, s->root, &d, &d, &wins, &nwins)) 
		return;
	for(uint8_t j = 0; j < nwins; j++) 
	{
		XWindowAttributes a;
		XGetWindowAttributes(D, wins[j], &a);
		if(!a.override_redirect && (a.map_state==IsViewable))
			make_new_client(wins[j], s);
	}
	XFree(wins);
}

__attribute__((cold))
static inline void
setup_screen_elements(const uint8_t i)
{
	ScreenInfo * s=&jbwm.X.screens[i];
	s->screen = i;
	s->root = RootWindow(D, i);
	s->vdesk = 0;
	s->size.w=DisplayWidth(D, i);
	s->size.h=DisplayHeight(D, i);
}

__attribute__((cold))
static inline void
setup_gc(ScreenInfo *s)
{
	allocate_colors(s);
	unsigned long vm=GCFunction|GCSubwindowMode|GCLineWidth|GCForeground
		|GCBackground;
	XGCValues gv={.foreground=s->fg.pixel, .background=s->bg.pixel, 
		.function=GXinvert, .subwindow_mode=IncludeInferiors, 
		.line_width=JBWM_BORDER };
#if defined(USE_TBAR) && !defined(USE_XFT)
	gv.font=jbwm.X.font->fid;
	vm|=GCFont;
#endif//USE_TBAR&&!USE_XFT
	s->gc = XCreateGC(D, s->root, vm, &gv);
}

__attribute__((cold))
static void
setup_screen(const uint8_t i)
{
	ScreenInfo *s=&jbwm.X.screens[i];
	setup_screen_elements(i);
	setup_gc(s);
	setup_event_listeners(s->root);
	grab_keys_for_screen(s);
	/* scan all the windows on this screen */
	setup_clients(s);
#ifdef EWMH
	setup_ewmh_for_screen(s);
#endif//EWMH
}

__attribute__((cold))
static inline void
setup_screens(void)
{
	/* Now set up each screen in turn: jbwm.X.num_screens is used 
	   in scanning windows (XQueryTree) */
	uint8_t i = jbwm.X.num_screens = ScreenCount(D);
	jbwm.X.screens = malloc(i * sizeof(ScreenInfo));
	while(i--) setup_screen(i);
}

int
handle_xerror(Display * dpy __attribute__((unused)), XErrorEvent * e)
{
        if((e->error_code==BadAccess) && (e->request_code 
                == X_ChangeWindowAttributes)) ERROR("ROOT");
        return 0; // Ignore everything else.         
}

__attribute__((cold))
static inline void
setup_display(void)
{
	if(!(D=XOpenDisplay(NULL)))
		ERROR("DISPLAY");
	XSetErrorHandler(handle_xerror);
#ifdef EWMH
	ewmh_init();
#endif//EWMH
#ifdef USE_TBAR
	/* Fonts only needed with title bars */
	setup_fonts();
#endif /* USE_TBAR */
	jbwm.X.cursor = XCreateFontCursor(D, XC_fleur);
	setup_screens();
}

int
main(
#ifdef USE_ARGV
	int argc, char **argv)
#else//!USE_ARGV
	void)
#endif//USE_ARGV
{
	jbwm.keymasks.grab = GRAB_MASK;
	jbwm.keymasks.mod = MOD_MASK;
#ifdef USE_ARGV
	parse_argv(argc, argv);
#endif//USE_ARGV
	setup_display();
	main_event_loop();
	return 0;
}
