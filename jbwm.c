// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "jbwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "JBWMScreen.h"
#include "config.h"
#include "ewmh.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include "screen.h"
#include "util.h"
// Macros:
#define ENV(e) JBWM_ENV_##e
/* Used for overriding the default WM modifiers */
__attribute__((warn_unused_result))
static uint16_t parse_modifiers(char * arg)
{
	JBWM_LOG("parse_modifiers()");
	const size_t s = strlen(arg);
	if (s > 3) { // switch based on the 4th character
		switch(arg[3]) {
		case 'f': // shift
			return ShiftMask;
		case 'k': // lock
			return LockMask;
		case 't': // control
		case 'l': // ctrl
			return ControlMask;
		case '2': // mod2
			return Mod2Mask;
		case '3': // mod3
			return Mod3Mask;
		case '4': // mod4
			return Mod4Mask;
		case '5': // mod5
			return Mod5Mask;
		}
	}
	// everything else becomes mod1
	return Mod1Mask;
}
void jbwm_parse_command_line(const uint8_t argc, char **argv)
{
	JBWM_LOG("parse_argv(%d,%s...)", argc, argv[0]);
	static const char optstring[] = "1:2:b:d:F:f:hs:Vv";
	int8_t opt;
#define OVERRIDE(x) setenv(ENV(x), optarg, 1);
	while((opt=getopt(argc, argv, optstring)) != -1)
		switch (opt) {
		case '1':
			jbwm_set_grab_mask(parse_modifiers(optarg));
			break;
		case '2':
			jbwm_set_mod_mask(parse_modifiers(optarg));
			break;
		case 'b':
			OVERRIDE(BG);
			break;
		case 'd':
			OVERRIDE(DISPLAY);
			break;
		case 'F':
			OVERRIDE(FONT);
			break;
		case 'f':
			OVERRIDE(FG);
			break;
		case 's':
			OVERRIDE(FC);
			break;
		case 'V':
		case 'v':
			printf("%s version %s\n", argv[0], VERSION);
			exit(0);
		default:
			printf("%s -[%s]\n", argv[0], optstring);
			exit(1);
		}
}
__attribute__((noreturn))
void jbwm_error(const char * msg)
{
	perror(msg);
	exit(1);
}
static void allocate_colors(Display * d, struct JBWMScreen * restrict s)
{
	const uint8_t n = s->screen;
#define PIX(field, env) s->pixels.field = jbwm_get_pixel(d, n, getenv(env));
	PIX(bg, ENV(BG));
	PIX(fc, ENV(FC));
	PIX(fg, ENV(FG));
#ifdef JBWM_USE_TITLE_BAR
	PIX(close, ENV(CLOSE));
	PIX(resize, ENV(RESIZE));
	PIX(shade, ENV(SHADE));
	PIX(stick, ENV(STICK));
#endif//JBWM_USE_TITLE_BAR
#undef PIX
}
static bool check_redirect(Display * d, const Window w)
{
	XWindowAttributes a;
	XGetWindowAttributes(d, w, &a);
	JBWM_LOG("check_redirect(0x%x): override_redirect: %s, "
		"map_state: %s", (int)w,
		a.override_redirect ? "true" : "false",
		a.map_state == IsViewable ? "IsViewable"
		: "not IsViewable");
	return (!a.override_redirect && (a.map_state == IsViewable));
}
// Free returned data with XFree()
static Window * get_windows(Display * dpy, const Window root,
	uint16_t * win_count)
{
	Window * w, d;
	unsigned int n;
	XQueryTree(dpy, root, &d, &d, &w, &n);
	*win_count = n;
	return (Window *)w;
}
static void setup_clients(Display * d, struct JBWMScreen * s)
{
	uint16_t n;
	Window * w = get_windows(d, s->root, &n);
	JBWM_LOG("Started with %d clients", n);
	while (n--)
		if (check_redirect(d, w[n])) {
			JBWM_LOG("Starting client %d, window 0x%x",
				n, (int)w[n]);
			jbwm_new_client(d, s, w[n]);
		}
	XFree(w);
}
static void setup_screen_elements(Display * d, const uint8_t i)
{
	struct JBWMScreen * s = jbwm_get_screens();
	s->screen = i;
	s->vdesk = 0;
	s->root = RootWindow(d, i);
	s->size.width = DisplayWidth(d, i);
	s->size.height = DisplayHeight(d, i);
}
static void setup_gc(Display * d, struct JBWMScreen * s)
{
	allocate_colors(d, s);
	s->gc = XCreateGC(d, s->root, GCFunction | GCSubwindowMode
		| GCLineWidth | GCForeground | GCBackground,
		&(XGCValues){.foreground = s->pixels.fg,
		.background = s->pixels.bg, .function = GXxor,
		.subwindow_mode = IncludeInferiors, .line_width = 1
	});

}
static void setup_event_listeners(Display * d, const Window root)
{
	enum {
		EMASK = SubstructureRedirectMask | SubstructureNotifyMask |
			EnterWindowMask | PropertyChangeMask
			| ColormapChangeMask
	};
	XChangeWindowAttributes(d, root, CWEventMask,
		&(XSetWindowAttributes){.event_mask = EMASK });
}
void jbwm_init_screen(Display * d, const uint8_t i)
{
	struct JBWMScreen * s = &jbwm_get_screens()[i];
	setup_screen_elements(d, i);
	setup_gc(d, s);
	setup_event_listeners(d, s->root);
	jbwm_grab_screen_keys(d, s);
	/* scan all the windows on this screen */
	setup_clients(d, s);
	jbwm_ewmh_init_screen(s);
}
void jbwm_set_defaults(void)
{
#define SETENV(i) setenv(ENV(i), JBWM_DEF_##i, 0)
	SETENV(FG); SETENV(FC); SETENV(BG); SETENV(TERM);
#ifdef JBWM_USE_TITLE_BAR
	SETENV(CLOSE); SETENV(RESIZE); SETENV(SHADE);
	SETENV(STICK); SETENV(FONT);
#endif//JBWM_USE_TITLE_BAR
#undef SETENV
}
