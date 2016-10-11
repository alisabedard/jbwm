// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "JBWMEnv.h"

#include "config.h"
#include "events.h"
#include "ewmh.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <X11/cursorfont.h>
#include <X11/Xproto.h>

#ifdef STDIO
#include <stdio.h>
#endif//STDIO

// Main application data structure.
struct JBWMEnv jbwm;

static void print(const size_t sz, const char * buf)
{
	if (write(1, buf, sz) == -1)
		abort();
}

#ifdef USE_ARGV

/* Used for overriding the default WM modifiers */
__attribute__((warn_unused_result))
static uint16_t parse_modifiers(char * restrict arg)
{
	LOG("parse_modifiers()");
	size_t s = 0; // strlen
	while(arg[++s])
		  ;
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

static void parse_argv(uint8_t argc, char **argv)
{
	LOG("parse_argv(%d,%s...)", argc, argv[0]);
	static const char optstring[] = "1:2:b:d:F:f:hs:V";
	int8_t opt;
	while((opt=getopt(argc, argv, optstring)) != -1) {
		switch (opt) {
		case '1':
			jbwm.keymasks.grab = parse_modifiers(optarg);
			break;
		case '2':
			jbwm.keymasks.mod = parse_modifiers(optarg);
			break;
		case 'b':
			setenv(JBWM_ENV_BG, optarg, 1);
			break;
		case 'd':
			setenv(JBWM_ENV_DISPLAY, optarg, 1);
			break;
		case 'F':
			setenv(JBWM_ENV_FONT, optarg, 1);
			break;
		case 'f':
			setenv(JBWM_ENV_FG, optarg, 1);
			break;
		case 's':
			setenv(JBWM_ENV_FC, optarg, 1);
			break;
		case 'V':
			print(sizeof(VERSION), VERSION);
			print(1, "\n");
			exit(0);
		default: { // usage
			size_t l = 0;
			while (argv[0][++l]);
			print(l, *argv);
			print(4, " -[ ");
			print(sizeof(optstring), optstring);
			print(2, "]\n");
			exit(1);
		}
		}
	}
}
#else//!USE_ARGV
#define parse_argv(a, b)
#endif//USE_ARGV

__attribute__((noreturn))
static void jbwm_error(const char * restrict msg)
{
	size_t count = 0;
	while (msg[++count]);
	print(count, msg);
	print(1, "\n");
	exit(1);
}

#ifdef USE_TBAR
static void setup_fonts(void)
{
	char * font = getenv(JBWM_ENV_FONT);
#ifdef USE_XFT
	jbwm.font = XftFontOpen(jbwm.d, DefaultScreen(jbwm.d),
		XFT_FAMILY, XftTypeString, font, XFT_SIZE,
		XftTypeDouble, JBWM_FONT_SIZE, NULL);
#else//!USE_XFT
	jbwm.font = XLoadQueryFont(jbwm.d, font);
#endif//USE_XFT
	if (!jbwm.font)
		jbwm_error(JBWM_ENV_FONT);
}
#else//!USE_TBAR
#define setup_fonts()
#endif//USE_TBAR

static void setup_event_listeners(const jbwm_window_t root)
{
	XChangeWindowAttributes(jbwm.d, root, CWEventMask,
		&(XSetWindowAttributes){.event_mask = SubstructureRedirectMask
		| SubstructureNotifyMask | EnterWindowMask | PropertyChangeMask
		| ColormapChangeMask});
}

static void allocate_colors(struct JBWMScreen * restrict s)
{
	const uint8_t n = s->screen;
	s->pixels.fg=jbwm_get_pixel(n, getenv(JBWM_ENV_FG));
	s->pixels.bg=jbwm_get_pixel(n, getenv(JBWM_ENV_BG));
	s->pixels.fc=jbwm_get_pixel(n, getenv(JBWM_ENV_FC));
#ifdef USE_TBAR
	s->pixels.close = jbwm_get_pixel(n, getenv(JBWM_ENV_CLOSE));
	s->pixels.resize = jbwm_get_pixel(n, getenv(JBWM_ENV_RESIZE));
	s->pixels.shade = jbwm_get_pixel(n, getenv(JBWM_ENV_SHADE));
	s->pixels.stick = jbwm_get_pixel(n, getenv(JBWM_ENV_STICK));
#endif//USE_TBAR
}

static bool check_redirect(const jbwm_window_t w)
{
	XWindowAttributes a;
	XGetWindowAttributes(jbwm.d, w, &a);
	return (!a.override_redirect && (a.map_state == IsViewable));
}

static void setup_clients(struct JBWMScreen * restrict s)
{
	Window * w, d; // don't use jbwm_window_t here
	unsigned int n;
	// XQueryTree depends on 64-bit types
	if (!XQueryTree(jbwm.d, s->root, &d, &d, &w, &n))
		return; // failed
	while (n--)
		if (check_redirect(w[n]))
			jbwm_new_client(w[n], s);
	XFree(w);
}

static void setup_screen_elements(const uint8_t i)
{
	struct JBWMScreen *restrict s = &jbwm.s[i];
	s->screen = i;
	Display * d = jbwm.d;
	s->root = RootWindow(d, i);
	s->vdesk = 0;
	s->size.w = DisplayWidth(d, i);
	s->size.h = DisplayHeight(d, i);
}

static void setup_gc(struct JBWMScreen * restrict s)
{
	allocate_colors(s);
	unsigned long vm = GCFunction | GCSubwindowMode
		| GCLineWidth | GCForeground | GCBackground;
	XGCValues gv = {.foreground = s->pixels.fg,
		.background = s->pixels.bg,
		.function = GXxor,
		.subwindow_mode = IncludeInferiors,
		.line_width = 1
	};
#if defined(USE_TBAR) && !defined(USE_XFT)
	gv.font = jbwm.font->fid;
	vm |= GCFont;
#endif//USE_TBAR&&!USE_XFT
	s->gc = XCreateGC(jbwm.d, s->root, vm, &gv);
}

static void setup_screen(const uint8_t i)
{
	struct JBWMScreen *s = &jbwm.s[i];
	setup_screen_elements(i);
	setup_gc(s);
	setup_event_listeners(s->root);
	grab_keys_for_screen(s);
	/* scan all the windows on this screen */
	setup_clients(s);
	setup_ewmh_for_screen(s);
}

__attribute__((pure))
static int handle_xerror(Display * restrict dpy __attribute__ ((unused)),
	XErrorEvent * restrict e)
{
	if ((e->error_code == BadAccess)
	    && (e->request_code == X_ChangeWindowAttributes)) {
		jbwm_error("ROOT");
	}
	LOG("xerror: %d, %d\n", e->error_code, e->request_code);
	return 0; // Ignore everything else.
}

static void jbwm_set_defaults(void)
{
	setenv(JBWM_ENV_FG, JBWM_DEF_FG, 0);
	setenv(JBWM_ENV_FC, JBWM_DEF_FC, 0);
	setenv(JBWM_ENV_BG, JBWM_DEF_BG, 0);
	setenv(JBWM_ENV_TERM, JBWM_DEF_TERM, 0);
#ifdef USE_TBAR
	setenv(JBWM_ENV_CLOSE, JBWM_DEF_CLOSE, 0);
	setenv(JBWM_ENV_RESIZE, JBWM_DEF_RESIZE, 0);
	setenv(JBWM_ENV_SHADE, JBWM_DEF_SHADE, 0);
	setenv(JBWM_ENV_STICK, JBWM_DEF_STICK, 0);
	setenv(JBWM_ENV_FONT, JBWM_DEF_FONT, 0);
#endif//USE_TBAR
}

int main(
#ifdef USE_ARGV
		int argc, char **argv)
#else//!USE_ARGV
		void)
#endif//USE_ARGV
{
	jbwm_set_defaults();
	jbwm.keymasks.grab = GRAB_MASK;
	jbwm.keymasks.mod = MOD_MASK;
	parse_argv(argc, argv);
	if (!(jbwm.d = XOpenDisplay(NULL)))
		jbwm_error(JBWM_ENV_DISPLAY);
	XSetErrorHandler(handle_xerror);
	ewmh_init();
	/* Fonts only needed with title bars */
	setup_fonts();
	jbwm.cursor = XCreateFontCursor(jbwm.d, XC_fleur);
	uint8_t i = ScreenCount(jbwm.d);
	struct JBWMScreen s[i]; // remains in scope till exit.
	jbwm.s = s;
	while (i--)
		setup_screen(i);
	main_event_loop();
	return 0;
}
