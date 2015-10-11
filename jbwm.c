/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2012, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include <signal.h>
#include <X11/cursorfont.h>
#include <unistd.h>
#include "jbwm.h"

/* Main application data structure.  */
JBWMEnvironment jbwm;

static void
initialize_JBWMEnvironment_keymasks(void)
{
	jbwm.keymasks.numlock = LOCK_MASK;
	jbwm.keymasks.grab = GRAB_MASK;
	jbwm.keymasks.mod = MOD_MASK;
}

static void
initialize_JBWMEnvironment(void)
{
	jbwm.initialising = None;
	initialize_JBWMEnvironment_keymasks();
	jbwm.titlebar.initialized = False;
}

/* These are difficult to consolidate in
   JBWMEnvironment structure, as memory would
   have to be manually allocated for each.  */
/* Things that affect user interaction */
GlobalOptions opt = {
	DEF_FONT,
	{DEF_FG, DEF_BG, DEF_FC},
	DEF_TERM
};

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
	"mod5", Mod5Mask}};
	const char *delim = ",+";
	char *tmp;
	unsigned int ret = 0;
	for(tmp = strtok(s, delim); tmp; tmp = strtok(NULL, delim))
	{
		ubyte i = 0;

		while(i++ < 8)
			if(!strcmp(modifiers[i].name, tmp))
				ret |= modifiers[i].mask;
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
parse_command_line_args(int argc, char **argv)
{
	int i;

	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] != '-')
			continue;
		switch (argv[i][1])
		{
#define JBG argv[++i]
#define SETJBG(arg) arg = ARG
#define SETNJBG(arg) arg = atoi(ARG)
		case 'd':
			setenv("DISPLAY", JBG, 1);
			break;
		case 'F':
			SETJBG(opt.font);
			break;
		case 'f':
			SETJBG(opt.color.fg);
			break;
		case 'b':
			SETJBG(opt.color.bg);
			break;
		case 'c':
			SETJBG(opt.color.fc);
			break;
		case 'T':
			SETJBG(opt.term);
			break;
		case 'A':
			process_app_class_options(JBG);
			break;
		case 'g':
			parse_geometry(JBG);
			break;
		case '1':
			jbwm.keymasks.grab = parse_modifiers(JBG);
			break;
		case '2':
			jbwm.keymasks.mod = parse_modifiers(JBG);
			break;
		case 'V':
			LOG_INFO("jbwm version " VERSION "\n");
			exit(0);
		default:
			fprintf(stderr,
				"jbwm -[d:F:f:b:v:T:w:s:A:g:Vv:S1:2:]\n");
			exit(0);
		}
	}
}
#endif /* USE_ARGV */

static void
setup_fonts(void)
{
#ifdef USE_XFT
#define FONTOPEN(f) XftFontOpenName(jbwm.X.dpy, DefaultScreen(jbwm.X.dpy), f)
#else /* !USE_XFT */
#define FONTOPEN(f) XLoadQueryFont(jbwm.X.dpy, f)
#endif /* USE_XFT */

	jbwm.X.font =
		opt.font ? FONTOPEN(opt.font) : FONTOPEN(DEF_FONT);
	if(!jbwm.X.font)
		LOG_ERROR("couldn't find a font to use: "
#ifdef USE_ARGV
			"try starting with -f fontname\n"
#else
			"set available font in config.h and rebuild"
#endif /* USE_ARGV */
			);
}

void
jbwm_grab_button(Window w, unsigned int mask, unsigned int button)
{
	do
	{
#define GRAB(mod) XGrabButton(jbwm.X.dpy, button, mask | mod, w, False,\
	ButtonMask, GrabModeAsync, GrabModeSync, None, None)
		GRAB(0);
		GRAB(LockMask);
		GRAB(Mod2Mask);	/* Grab for ALT in client window */
	}
	while(0);
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

static void
allocate_colors(const ubyte i)
{
	XColor dummy;

#define COLORALLOC(item)\
	XAllocNamedColor(jbwm.X.dpy, DefaultColormap(jbwm.X.dpy, i),\
		opt.color.item, &jbwm.X.screens[i].item, &dummy);
	COLORALLOC(fg);
	COLORALLOC(bg);
	COLORALLOC(fc);
}

static void
setup_gc_parameters(XGCValues * gv, const ubyte i)
{
	allocate_colors(i);
	gv->foreground = jbwm.X.screens[i].fg.pixel;
	gv->background = jbwm.X.screens[i].bg.pixel;
	/* set up GC parameters - same for each screen */
	gv->function = GXinvert;
	gv->subwindow_mode = IncludeInferiors;
	gv->line_width = JBWM_BORDER_WIDTH;
#ifndef USE_XFT
	gv->font = jbwm.X.font->fid;
#endif /* ! USE_XFT */
}

static void
setup_gc(const ubyte i)
{
	XGCValues gv;

	setup_gc_parameters(&gv, i);
	jbwm.X.screens[i].gc =
		XCreateGC(jbwm.X.dpy, jbwm.X.screens[i].root,
		GCFunction | GCSubwindowMode | GCLineWidth
#ifndef USE_XFT
		| GCFont
#endif /* ! USE_XFT */
		, &gv);
}

static void
setup_each_client(int i, int j, Window * wins)
{
	XWindowAttributes winattr;

	if(!wins)
		return;
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

	XQueryTree(jbwm.X.dpy, jbwm.X.screens[i].root, &dw1, &dw2,
		&wins, &nwins);
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
setup_display_per_screen(int i)
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
	int e_dummy;

	jbwm.X.have_shape =
		XShapeQueryExtension(jbwm.X.dpy, &jbwm.X.shape_event,
		&e_dummy);
}
#endif /* USE_SHAPE */

static void
setup_screens(void)
{
	/* now set up each screen in turn */
	jbwm.X.num_screens = ScreenCount(jbwm.X.dpy);
	jbwm.X.screens =
		malloc(jbwm.X.num_screens * sizeof(ScreenInfo));
	{
		/* used in scanning windows (XQueryTree) */
		ubyte i = jbwm.X.num_screens;

		while(i--)
			setup_display_per_screen(i);
	}
}

static void
setup_display(void)
{
	const char *dpy_env = getenv("DISPLAY");
	if(!(jbwm.X.dpy = XOpenDisplay(dpy_env ? dpy_env : ":0")))
		exit(1);
	XSetErrorHandler(handle_xerror);
	setup_fonts();
	jbwm.X.cursor = XCreateFontCursor(jbwm.X.dpy, XC_fleur);
#ifdef USE_SHAPE
	setup_shape();
#endif /* USE_SHAPE */
	setup_screens();
}

int
main(int argc
#ifndef USE_ARGV
	__attribute__ ((unused))
#endif /* not USE_ARGV */
	, char **argv
#ifndef USE_ARGV
	__attribute__ ((unused))
#endif /* not USE_ARGV */
	)
{
	initialize_JBWMEnvironment();
#ifdef USE_ARGV
	parse_command_line_args(argc, argv);
#endif /* USE_ARGV */
	setup_display();
	main_event_loop();

	return 1;
}
