// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

// Main application data structure.
JBWMEnvironment jbwm;

/* Several following functions are declared cold as they are only used on
program startup.  */

#ifdef USE_ARGV

/* Used for overriding the default WM modifiers */
__attribute__ ((cold))
static unsigned int parse_modifiers(char *arg)
{
	LOG("parse_modifiers()");
/* *INDENT-OFF* */
	const struct {
		const char *name;
		const unsigned int mask;
	} m[] = { { "shift", ShiftMask}, { "lock", LockMask},
		{ "control", ControlMask}, { "mod", Mod1Mask},
		{ "mod1", Mod1Mask}, { "mod2", Mod2Mask}, { "mod3", Mod3Mask},
		{ "mod4", Mod4Mask}, { "mod5", Mod5Mask} };
/* *INDENT-ON* */

	for (uint8_t i = 0; i < 9; i++)
		if (!strcmp(m[i].name, arg))
			return m[i].mask;

	return 0;
}

static struct {
	char *fg, *bg, *fc;
} jbwmopt;

static void parse_argv(int argc, char **argv)
{
	LOG("parse_argv(%d,%s...)", argc, argv[0]);
	const char *optstring = "1:2:b:d:f:s:V";

	for (int opt; (opt = getopt(argc, argv, optstring)) != -1;) {
		switch (opt) {
		case '1':
			jbwm.keymasks.grab = parse_modifiers(optarg);
			break;

		case '2':
			jbwm.keymasks.mod = parse_modifiers(optarg);
			break;

		case 'b':
			jbwmopt.bg = optarg;
			break;

		case 'd':
			setenv("DISPLAY", optarg, 1);
			break;

		case 'f':
			jbwmopt.fg = optarg;
			break;

		case 's':
			jbwmopt.fc = optarg;
			break;
#ifdef STDIO

		case 'V':
			puts(VERSION);
			exit(EX_OK);

		default:	/* Usage */
			fprintf(stderr, "%s [%s]\n", argv[0], optstring);
#else//!STDIO

		default:
#endif//STDIO
			exit(EX_USAGE);
		}
	}
}
#else//!USE_ARGV
#define parse_argv(a, b)
#endif//USE_ARGV

#ifdef USE_TBAR
static void setup_fonts(void)
{
#ifdef USE_XFT
	jbwm.font =
	    XftFontOpen(D, DefaultScreen(D), XFT_FAMILY, XftTypeString,
			DEF_FONT, XFT_SIZE, XftTypeDouble, FONT_SIZE, NULL);
#else//!USE_XFT
	jbwm.font = XLoadQueryFont(D, DEF_FONT);

	if (!jbwm.font)
		jbwm.font = XLoadQueryFont(D, FALLBACK_FONT);

#endif//USE_XFT

	if (!jbwm.font)
		ERROR("FONT");
}
#else//!USE_TBAR
#define setup_fonts()
#endif//USE_TBAR

void
jbwm_grab_button(const Window w, const unsigned int mask,
		 const unsigned int btn)
{
	XGrabButton(D, btn, mask, w, false,
		    ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
		    GrabModeSync, None, None);
}

__attribute__ ((cold))
static void setup_event_listeners(const Window root)
{
/* *INDENT-OFF* */
	XSetWindowAttributes attr = {.event_mask = SubstructureRedirectMask
		| SubstructureNotifyMask | EnterWindowMask | PropertyChangeMask
		| ColormapChangeMask };
/* *INDENT-ON* */
	XChangeWindowAttributes(D, root, CWEventMask, &attr);
}

__attribute__ ((cold))
static void allocate_colors(ScreenInfo * restrict s)
{
	XColor nc;
	const Colormap cm = DefaultColormap(D, s->screen);
#ifdef USE_ARGV
	XAllocNamedColor(D, cm, jbwmopt.fg ? jbwmopt.fg : DEF_FG, &s->fg, &nc);
	XAllocNamedColor(D, cm, jbwmopt.bg ? jbwmopt.bg : DEF_BG, &s->bg, &nc);
	XAllocNamedColor(D, cm, jbwmopt.fc ? jbwmopt.fc : DEF_FC, &s->fc, &nc);
#else//!USE_ARGV
	XAllocNamedColor(D, cm, DEF_FG, &s->fg, &nc);
	XAllocNamedColor(D, cm, DEF_BG, &s->bg, &nc);
	XAllocNamedColor(D, cm, DEF_FC, &s->fc, &nc);
#endif//USE_ARGV
}

static void setup_clients(ScreenInfo * restrict s)
{
	unsigned int nwins;
	Window *wins;
	Window d;

	if (!XQueryTree(D, s->root, &d, &d, &wins, &nwins))
		return;

	while (nwins--) {
		XWindowAttributes a;
		XGetWindowAttributes(D, wins[nwins], &a);

		if (!a.override_redirect && (a.map_state == IsViewable))
			make_new_client(wins[nwins], s);
	}

	XFree(wins);
}

__attribute__ ((cold))
static void setup_screen_elements(const uint8_t i)
{
	ScreenInfo *restrict s = &jbwm.screens[i];
	s->screen = i;
	s->root = RootWindow(D, i);
	s->vdesk = 0;
	s->size.w = DisplayWidth(D, i);
	s->size.h = DisplayHeight(D, i);
}

__attribute__ ((cold))
static void setup_gc(ScreenInfo * s)
{
	allocate_colors(s);
	unsigned long vm =
	    GCFunction | GCSubwindowMode | GCLineWidth | GCForeground |
	    GCBackground;
	XGCValues gv = {.foreground = s->fg.pixel, .background = s->bg.pixel,
		.function = GXxor, .subwindow_mode = IncludeInferiors,
		.line_width = JBWM_BORDER
	};
#if defined(USE_TBAR) && !defined(USE_XFT)
	gv.font = jbwm.font->fid;
	vm |= GCFont;
#endif//USE_TBAR&&!USE_XFT
	s->gc = XCreateGC(D, s->root, vm, &gv);
}

__attribute__ ((cold))
static void setup_screen(const uint8_t i)
{
	ScreenInfo *s = &jbwm.screens[i];
	setup_screen_elements(i);
	setup_gc(s);
	setup_event_listeners(s->root);
	grab_keys_for_screen(s);
	/* scan all the windows on this screen */
	setup_clients(s);
	setup_ewmh_for_screen(s);
}

__attribute__ ((cold))
static void setup_screens(void)
{
	/* Now set up each screen in turn: jbwm.num_screens is used
	   in scanning windows (XQueryTree) */
	uint8_t i = ScreenCount(D);
	jbwm.screens = malloc(i * sizeof(ScreenInfo));

	while (i--)
		setup_screen(i);
}


int handle_xerror(Display * dpy __attribute__ ((unused)), XErrorEvent * e)
{
	if ((e->error_code == BadAccess)
	    && (e->request_code == X_ChangeWindowAttributes))
		ERROR("ROOT");

	return 0;		// Ignore everything else.
}

int main(
#ifdef USE_ARGV
		int argc, char **argv)
#else				//!USE_ARGV
		void)
#endif				//USE_ARGV
{
	jbwm.keymasks.grab = GRAB_MASK;
	jbwm.keymasks.mod = MOD_MASK;
	parse_argv(argc, argv);

	if (!(D = XOpenDisplay(NULL)))
		ERROR("DISPLAY");

	XSetErrorHandler(handle_xerror);
	ewmh_init();
	/* Fonts only needed with title bars */
	setup_fonts();
	jbwm.cursor = XCreateFontCursor(D, XC_fleur);
	setup_screens();
	main_event_loop();
	return 0;
}
