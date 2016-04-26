// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <string.h>
#ifdef USE_ARGV
#include <unistd.h>
#endif//USE_ARGV
#include <X11/cursorfont.h>
#include "config.h"
#include "events.h"
#include "ewmh.h"
#include "jbwmenv.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include "util.h"

// Main application data structure.
JBWMEnvironment jbwm;

typedef struct {
	// Color names:
	char *fg, *bg, *fc;
} Options;

#ifdef USE_ARGV

/* Used for overriding the default WM modifiers */
__attribute__((warn_unused_result))
static unsigned int parse_modifiers(char * restrict arg)
{
	LOG("parse_modifiers()");
/* *INDENT-OFF* */
	const struct {
		const char *name;
		const uint16_t mask;
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

static void parse_argv(int argc, char **argv, Options * restrict o)
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
			o->bg = optarg;
			break;

		case 'd':
			setenv("DISPLAY", optarg, 1);
			break;

		case 'f':
			o->fg = optarg;
			break;

		case 's':
			o->fc = optarg;
			break;
#ifdef STDIO
		case 'V':
			puts(VERSION);
			exit(0);

		default:	/* Usage */
			fprintf(stderr, "%s [%s]\n", argv[0], optstring);
#else//!STDIO

		default:
#endif//STDIO
			exit(1);
		}
	}
}
#else//!USE_ARGV
#define parse_argv(a, b, o)
#endif//USE_ARGV

#ifdef USE_TBAR
static void setup_fonts(void)
{
#ifdef USE_XFT
	jbwm.font = XftFontOpen(jbwm.dpy, DefaultScreen(jbwm.dpy),
		XFT_FAMILY, XftTypeString, DEF_FONT, XFT_SIZE,
		XftTypeDouble, FONT_SIZE, NULL);
#else//!USE_XFT
	jbwm.font = XLoadQueryFont(jbwm.dpy, DEF_FONT);

	if (!jbwm.font)
		jbwm.font = XLoadQueryFont(jbwm.dpy, FALLBACK_FONT);

#endif//USE_XFT

	if (!jbwm.font)
		ERROR("Cannot load any font");
}
#else//!USE_TBAR
#define setup_fonts()
#endif//USE_TBAR

#ifdef USE_SHAPE
#include <X11/extensions/shape.h>
#endif
static void setup_event_listeners(const Window root)
{
	XChangeWindowAttributes(jbwm.dpy, root, CWEventMask,
		&(XSetWindowAttributes){.event_mask = SubstructureRedirectMask
		| SubstructureNotifyMask | EnterWindowMask | PropertyChangeMask
		| ColormapChangeMask});
}

static void allocate_colors(ScreenInfo * restrict s,
	Options * restrict o)
{
	s->pixels.fg=pixel(s->screen, o->fg);
	s->pixels.bg=pixel(s->screen, o->bg);
	s->pixels.fc=pixel(s->screen, o->fc);
}

static void setup_clients(ScreenInfo * restrict s)
{
	unsigned int nwins;
	Window *wins;

	if (!XQueryTree(jbwm.dpy, s->root, &(Window){0}, 
		&(Window){0}, &wins, &nwins))
		return;

	while (nwins--) {
		XWindowAttributes a;
		XGetWindowAttributes(jbwm.dpy, wins[nwins], &a);

		if (!a.override_redirect && (a.map_state == IsViewable))
			make_new_client(wins[nwins], s);
	}

	XFree(wins);
}

static void setup_screen_elements(const uint8_t i)
{
	ScreenInfo *restrict s = &jbwm.screens[i];
	s->screen = i;
	s->root = RootWindow(jbwm.dpy, i);
	s->vdesk = 0;
	s->size.w = DisplayWidth(jbwm.dpy, i);
	s->size.h = DisplayHeight(jbwm.dpy, i);
}

static void setup_gc(ScreenInfo * restrict s, Options * restrict o)
{
	allocate_colors(s, o);
	unsigned long vm =
	    GCFunction | GCSubwindowMode | GCLineWidth | GCForeground |
	    GCBackground;
	XGCValues gv = {.foreground = s->pixels.fg, .background = s->pixels.bg,
		.function = GXxor, .subwindow_mode = IncludeInferiors,
		.line_width = JBWM_BORDER
	};
#if defined(USE_TBAR) && !defined(USE_XFT)
	gv.font = jbwm.font->fid;
	vm |= GCFont;
#endif//USE_TBAR&&!USE_XFT
	s->gc = XCreateGC(jbwm.dpy, s->root, vm, &gv);
}

static void setup_screen(const uint8_t i, Options * restrict o)
{
	ScreenInfo *s = &jbwm.screens[i];
	setup_screen_elements(i);
	setup_gc(s, o);
	setup_event_listeners(s->root);
	grab_keys_for_screen(s);
	/* scan all the windows on this screen */
	setup_clients(s);
	setup_ewmh_for_screen(s);
}

static void setup_screens(Options * restrict o)
{
	/* Now set up each screen in turn: jbwm.num_screens is used
	   in scanning windows (XQueryTree) */
	uint8_t i = ScreenCount(jbwm.dpy);
	jbwm.screens = malloc(i * sizeof(ScreenInfo));

	while (i--)
		setup_screen(i, o);
}


int handle_xerror(Display * dpy __attribute__ ((unused)), XErrorEvent * e)
{
	if ((e->error_code == BadAccess)
	    && (e->request_code == X_ChangeWindowAttributes)) {
		ERROR("ROOT");
	}

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
	Options o = {.fg=DEF_fg, .bg=DEF_bg, .fc=DEF_fc};
	parse_argv(argc, argv, &o);

	if (!(jbwm.dpy = XOpenDisplay(NULL)))
		ERROR("DISPLAY");

	XSetErrorHandler(handle_xerror);
	ewmh_init();
	/* Fonts only needed with title bars */
	setup_fonts();
	jbwm.cursor = XCreateFontCursor(jbwm.dpy, XC_fleur);
	setup_screens(&o);
	main_event_loop();
	return 0;
}
