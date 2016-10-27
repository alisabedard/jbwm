// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "JBWMEnv.h"
#include "client.h"
#include "config.h"
#include "events.h"
#include "ewmh.h"
#include "keys.h"
#include "log.h"
#include "new.h"
#include "util.h"
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xproto.h>
#ifdef STDIO
#include <stdio.h>
#endif//STDIO
// Main application data structure.
struct JBWMEnv jbwm;
static struct {
	struct JBWMScreen * screens;
#ifdef JBWM_USE_TITLE_BAR
#ifdef JBWM_USE_XFT
	XftFont * font;
#else//! JBWM_USE_XFT
	XFontStruct * font;
#endif//JBWM_USE_XFT
#endif//JBWM_USE_TITLE_BAR
} jbwm_data;
#ifdef JBWM_USE_TITLE_BAR
void * jbwm_get_font(void)
{
	return jbwm_data.font;
}
uint8_t jbwm_get_font_ascent(void)
{
	return jbwm_data.font->ascent;
}
uint8_t jbwm_get_font_descent(void)
{
	return jbwm_data.font->descent;
}
uint8_t jbwm_get_font_height(void)
{
	return jbwm_data.font->ascent + jbwm_data.font->descent;
}
#endif//JBWM_USE_TITLE_BAR
struct JBWMScreen * jbwm_get_screens(void)
{
	return jbwm_data.screens;
}
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
	JBWM_LOG("parse_modifiers()");
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
	JBWM_LOG("parse_argv(%d,%s...)", argc, argv[0]);
	static const char optstring[] = "1:2:b:d:F:f:hs:V";
	int8_t opt;
	while((opt=getopt(argc, argv, optstring)) != -1) {
		switch (opt) {
		case '1':
			jbwm_set_grab_mask(parse_modifiers(optarg));
			break;
		case '2':
			jbwm_set_mod_mask(parse_modifiers(optarg));
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
#ifdef JBWM_USE_TITLE_BAR
static void setup_fonts(void)
{
	char * font = getenv(JBWM_ENV_FONT);
#ifdef JBWM_USE_XFT
	jbwm_data.font = XftFontOpen(jbwm.d, DefaultScreen(jbwm.d),
		XFT_FAMILY, XftTypeString, font, XFT_SIZE,
		XftTypeDouble, JBWM_FONT_SIZE, NULL);
#else//!JBWM_USE_XFT
	jbwm_data.font = XLoadQueryFont(jbwm.d, font);
#endif//JBWM_USE_XFT
	if (!jbwm_data.font)
		jbwm_error(JBWM_ENV_FONT);
}
#else//!JBWM_USE_TITLE_BAR
#define setup_fonts()
#endif//JBWM_USE_TITLE_BAR
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
#ifdef JBWM_USE_TITLE_BAR
	s->pixels.close = jbwm_get_pixel(n, getenv(JBWM_ENV_CLOSE));
	s->pixels.resize = jbwm_get_pixel(n, getenv(JBWM_ENV_RESIZE));
	s->pixels.shade = jbwm_get_pixel(n, getenv(JBWM_ENV_SHADE));
	s->pixels.stick = jbwm_get_pixel(n, getenv(JBWM_ENV_STICK));
#endif//JBWM_USE_TITLE_BAR
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
	struct JBWMScreen * restrict s = jbwm_data.screens;
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
#if defined(JBWM_USE_TITLE_BAR) && !defined(JBWM_USE_XFT)
	gv.font = jbwm_data.font->fid;
	vm |= GCFont;
#endif//JBWM_USE_TITLE_BAR&&!JBWM_USE_XFT
	s->gc = XCreateGC(jbwm.d, s->root, vm, &gv);
}
static void setup_screen(const uint8_t i)
{
	struct JBWMScreen *s = &jbwm_data.screens[i];
	setup_screen_elements(i);
	setup_gc(s);
	setup_event_listeners(s->root);
	jbwm_grab_screen_keys(s);
	/* scan all the windows on this screen */
	setup_clients(s);
	jbwm_ewmh_init_screen(s);
}
__attribute__((pure))
static int handle_xerror(Display * restrict dpy __attribute__ ((unused)),
	XErrorEvent * restrict e)
{
	if ((e->error_code == BadAccess)
	    && (e->request_code == X_ChangeWindowAttributes)) {
		jbwm_error("ROOT");
	}
	if (e->error_code == BadWindow) { // cleanup zombie windows
		struct JBWMClient * c = jbwm_get_client(e->resourceid);
		if (c) // match found
			jbwm_free_client(c);
	}
	JBWM_LOG("xerror: %d, %d\n", e->error_code, e->request_code);
	return 0; // Ignore everything else.
}
static void jbwm_set_defaults(void)
{
	setenv(JBWM_ENV_FG, JBWM_DEF_FG, 0);
	setenv(JBWM_ENV_FC, JBWM_DEF_FC, 0);
	setenv(JBWM_ENV_BG, JBWM_DEF_BG, 0);
	setenv(JBWM_ENV_TERM, JBWM_DEF_TERM, 0);
#ifdef JBWM_USE_TITLE_BAR
	setenv(JBWM_ENV_CLOSE, JBWM_DEF_CLOSE, 0);
	setenv(JBWM_ENV_RESIZE, JBWM_DEF_RESIZE, 0);
	setenv(JBWM_ENV_SHADE, JBWM_DEF_SHADE, 0);
	setenv(JBWM_ENV_STICK, JBWM_DEF_STICK, 0);
	setenv(JBWM_ENV_FONT, JBWM_DEF_FONT, 0);
#endif//JBWM_USE_TITLE_BAR
}
int main(
#ifdef USE_ARGV
		int argc, char **argv)
#else//!USE_ARGV
		void)
#endif//USE_ARGV
{
	jbwm_set_defaults();
	parse_argv(argc, argv);
	if (!(jbwm.d = XOpenDisplay(NULL)))
		jbwm_error(JBWM_ENV_DISPLAY);
	XSetErrorHandler(handle_xerror);
	jbwm_ewmh_init();
	/* Fonts only needed with title bars */
	setup_fonts();
	uint8_t i = ScreenCount(jbwm.d);
	struct JBWMScreen s[i]; // remains in scope till exit.
	jbwm_data.screens = s;
	while (i--)
		setup_screen(i);
	jbwm_event_loop();
	return 0;
}
