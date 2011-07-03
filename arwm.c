/*
 * arwm - Restructuring, optimization, and feature fork
 *        Copyright 2007, Jeffrey E. Bedard <antiright@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <arwm@6809.org.uk> see README for license and other details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include "arwm.h"


/* Main application data structure.  */
ARWMEnvironment arwm;

static void
initialize_ARWMEnvironment_keymasks(void)
{
	arwm.keymasks.numlock=0;
	arwm.keymasks.grab1=ControlMask | Mod1Mask;
	arwm.keymasks.grab2=Mod1Mask;
	arwm.keymasks.alt=ShiftMask;
}

static void
initialize_ARWMEnvironment_options(void)
{
	arwm.options.bw=DEF_BW;
#ifdef SNAP
	arwm.options.snap=ARWM_DEFAULT_SNAP;
#endif /* SNAP */
#ifdef SOLIDDRAG
	arwm.options.solid_drag=True;
#endif /* SOLIDDRAG */
}
static void
initialize_ARWMEnvironment(void)
{
	arwm.initialising=None;
	initialize_ARWMEnvironment_keymasks();
	initialize_ARWMEnvironment_options();
#ifdef TITLEBAR
	arwm.titlebar.initialized=False;
#endif /* TITLEBAR */
}

/* These are difficult to consolidate in
   ARWMEnvironment structure, as memory would
   have to be manually allocated for each.  */
/* Things that affect user interaction */
char *arwm_opt_font = DEF_FONT;
static const char *arwm_opt_display = "";
static const char *opt_fg = DEF_FG;
static const char *opt_bg = DEF_BG;
static const char *opt_fc = DEF_FC;
const char *opt_term[3] = {DEF_TERM, DEF_TERM, NULL};

Application *head_app = NULL;

/* Client tracking information */
Client *head_client = NULL;
Client *current = NULL;

static Application *
new_Application(void)
{
	Application *new = malloc(sizeof(Application));

	new->res_name = new->res_class = NULL;
	new->geometry_mask = 0;
	new->vdesk = -1;
	new->is_sticky = False;

	return new;
}

static void
process_app_class_options(const char * name)
{
	char *tmp;
	Application * new = new_Application();

	if ((tmp = strchr(name, '/')))
		*(tmp++) = 0;
	if (strlen(name) > 0)
	{
		new->res_name = malloc(strlen(name) + 1);
		strcpy(new->res_name, name);
	}
	if (tmp && strlen(tmp) > 0)
	{
		new->res_class = malloc(strlen(tmp) + 1);
		strcpy(new->res_class, tmp);
	}
	new->next = head_app;
	head_app = new;
}

static void
print_help(void)
{
	puts(
	"usage: arwm [-display display] [-term termprog]"
	" [-fn fontname] [-fg foreground] [-fc fixed] [-bg background]"
	" [-bw borderwidth] [-mask1 modifiers] [-mask2 modifiers]"
	" [-altmask modifiers] [-snap num] [-app name/class] [-g geometry]"
	" [-v vdesk] [-s] [-nosoliddrag] [-V]");
}

/* Used for overriding the default WM modifiers */
static unsigned int 
parse_modifiers(char *s)
{
	static struct
	{
		const char *name;
		unsigned int mask;
	}      modifiers[9] =
	{
		{ "shift", ShiftMask },
		{ "lock", LockMask },
		{ "control", ControlMask },
		{ "alt", Mod1Mask },
		{ "mod1", Mod1Mask },
		{ "mod2", Mod2Mask },
		{ "mod3", Mod3Mask },
		{ "mod4", Mod4Mask },
		{ "mod5", Mod5Mask }
	};
	char *tmp = strtok(s, ",+");
	unsigned int ret = 0;

	if (!tmp)
		return 0;
	do
	{
		int i;

		for (i = 0; i < 9; i++)
		{
			if (!strcmp(modifiers[i].name, tmp))
				ret |= modifiers[i].mask;
		}
	} while((tmp=strtok(NULL, ",+")));
	return ret;
}

static void
parse_command_line_args(int argc, char ** argv)
{
	int i;

	for (i = 1; i < argc; i++)
	{
#define ARGCMP(arg) (!strcmp(argv[i], arg) && ((i+1) < argc))
#define ARGNCMP(arg, n) (!strncmp(argv[i], arg, n))
#define TESTARG(arg) if(ARGCMP(arg))
		TESTARG("-fn")
			arwm_opt_font=argv[++i];
		else TESTARG("-display")
			arwm_opt_display = argv[++i];
		else TESTARG("-fg")
			opt_fg = argv[++i];
		else TESTARG("-bg")
			opt_bg = argv[++i];
		else TESTARG("-fc")
			opt_fc = argv[++i];
		else TESTARG("-bw")
			arwm.options.bw = atoi(argv[++i]);
		else TESTARG("-term")
		{
			opt_term[0] = argv[++i];
			opt_term[1] = opt_term[0];
		}
		else TESTARG("-snap")
		{
#ifdef SNAP
			arwm.options.snap = atoi(argv[++i]);
#else /* not SNAP */
			i++;
#endif /* SNAP */
		}
		else TESTARG("-app")
			process_app_class_options(argv[++i]);
		else TESTARG("-g")
		{
			int x, y;
			unsigned int w, h;

			if (!head_app)
				continue;
			head_app->geometry_mask = XParseGeometry( argv[++i], 
				&x, &y, &w, &h);
			/* Type conversions delayed to prevent warning,
			   following strict aliasing rules.  */
			head_app->geometry.x=x;
			head_app->geometry.y=y;
			head_app->geometry.width=w;
			head_app->geometry.height=h;
		}
		else TESTARG("-v")
		{
			int v = atoi(argv[++i]);

			if (head_app && valid_vdesk(v))
				head_app->vdesk = v;
		}
		else TESTARG("-s")
		{
			if (head_app)
				head_app->is_sticky = True;
		}
		else TESTARG("-mask1")
			arwm.keymasks.grab1=parse_modifiers(argv[++i]);
		else TESTARG("-mask2")
			arwm.keymasks.grab2=parse_modifiers(argv[++i]);
		else TESTARG("-altmask")
			arwm.keymasks.alt=parse_modifiers(argv[++i]);
#ifdef SOLIDDRAG
		else TESTARG("-nosoliddrag")
			arwm.options.solid_drag = 0;
#endif /* NOSOLIDDRAG */
		else TESTARG("-V")
		{
			LOG_INFO("arwm version " VERSION "\n");
			exit(0);
		}
		else
		{
			print_help();
			if(ARGNCMP("-h", 2) || ARGNCMP("--help", 3))
				exit(0);
		}
	}
	
}

static void
setup_each_signal_action(struct sigaction * act)
{
	sigaction(SIGTERM, act, NULL);
	sigaction(SIGINT, act, NULL);
	sigaction(SIGHUP, act, NULL);
}

static void
setup_signal_handlers(void)
{
	struct sigaction act;

	act.sa_handler = handle_signal;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	setup_each_signal_action(&act);
}


static void
setup_standard_atoms(void)
{
	/* Standard X protocol atoms */
	arwm.atoms.wm.state=GETATOM("WM_STATE");
	arwm.atoms.wm.protos=GETATOM("WM_PROTOCOLS");
	arwm.atoms.wm.delete=GETATOM("WM_DELETE_WINDOW");
}

static void
setup_vwm_atoms(void)
{
	arwm.atoms.vwm.desktop=GETATOM("_NET_WM_DESKTOP");
	arwm.atoms.vwm.state=GETATOM("_NET_WM_STATE");
	arwm.atoms.vwm.sticky=GETATOM("_NET_WM_STATE_STICKY");
        arwm.atoms.vwm.fullscreen=GETATOM("_NET_WM_STATE_FULLSCREEN");
	arwm_setup_ewmh_hints();
}

static void
setup_atoms(void)
{
	setup_standard_atoms();
	arwm.atoms.wm.cmapwins=GETATOM("WM_COLORMAP_WINDOWS");
	/* Motif atoms */
	arwm.atoms.mwm_hints =GETATOM("_XA_MWM_HINTS");
	/* ARWMH atoms */
	setup_vwm_atoms();
}

static void
setup_fonts(void)
{
	arwm.X.font = XftFontOpenName(arwm.X.dpy, 
			DefaultScreen(arwm.X.dpy),
			arwm_opt_font);
}

static void
setup_numlock_key_modifier(void)
{
	XModifierKeymap *modmap;
	int i;
	unsigned int j;

	/*
	 * find out which modifier is NumLock - we'll use this when grabbing
	 * every combination of modifiers we can think of
	 */
	modmap = XGetModifierMapping(arwm.X.dpy);
	for (i = 0; i < 8; i++)
		for (j = 0; j < (unsigned int)modmap->max_keypermod; j++)
			if (modmap->modifiermap[i * modmap->max_keypermod + j]
				== XKeysymToKeycode(arwm.X.dpy, XK_Num_Lock))
				arwm.keymasks.numlock=(1 << i);
	XFreeModifiermap(modmap);
}

void
arwm_grab_button(Window w, unsigned int mask, unsigned int button)
{
	do {
		XGrabButton(arwm.X.dpy, button, (mask), w, False, 
			ButtonMask, GrabModeAsync, GrabModeSync, 
			None, None); 
		XGrabButton(arwm.X.dpy, button, LockMask|(mask), 
			w, False, ButtonMask, GrabModeAsync, 
			GrabModeSync, None, None); 
		XGrabButton(arwm.X.dpy, button, arwm.keymasks.numlock|(mask), 
			w, False, ButtonMask, GrabModeAsync, GrabModeSync, 
			None, None); 
		XGrabButton(arwm.X.dpy, button, 
			arwm.keymasks.numlock|LockMask|(mask), w, False,
			ButtonMask, GrabModeAsync, GrabModeSync, None, None); 
	} while (0);
}

static void
setup_gc_parameters(XGCValues * gv)
{
	/* set up GC parameters - same for each screen */
	gv->function = GXinvert;
	gv->subwindow_mode = IncludeInferiors;
	gv->line_width = arwm.options.bw;
}
static void
setup_event_listeners(int i)
{
	XSetWindowAttributes attr;

	/* set up root window attributes 
		- same for each screen */
	attr.event_mask = ChildMask | EnterWindowMask
		| ColormapChangeMask
		| PropertyChangeMask ;

	XChangeWindowAttributes(arwm.X.dpy, arwm.X.screens[i].root, 
		CWEventMask, &attr);
}
static void
allocate_colors(int i)
{
	XColor dummy;
#define COLORALLOC(opt)\
	XAllocNamedColor(arwm.X.dpy, DefaultColormap(arwm.X.dpy, i), opt_##opt,\
		&arwm.X.screens[i].opt, &dummy);
	COLORALLOC(fg);
	COLORALLOC(bg);
	COLORALLOC(fc);
}

static void
setup_gc(int i)
{
	XGCValues gv;

	setup_gc_parameters(&gv);
	arwm.X.screens[i].invert_gc = XCreateGC(arwm.X.dpy, 
		arwm.X.screens[i].root, GCFunction 
		| GCSubwindowMode | GCLineWidth 
#ifndef USE_XFT
		| GCFont
#endif /* ! USE_XFT */
		, &gv);
}

static void
setup_cursors(void)
{
	arwm.X.cursors.move=XCreateFontCursor(arwm.X.dpy, XC_fleur);
	arwm.X.cursors.resize=XCreateFontCursor(arwm.X.dpy, XC_plus);
}

static void
setup_each_client(int i, int j, Window * wins)
{
	XWindowAttributes winattr;

	XGetWindowAttributes(arwm.X.dpy, wins[j], &winattr);
	if (!winattr.override_redirect && winattr.map_state
		== IsViewable)
		make_new_client(wins[j], &arwm.X.screens[i]);
}

static void
setup_clients(int i)
{
	Window dw1, dw2, *wins;
	unsigned int j, nwins;

	XQueryTree(arwm.X.dpy, arwm.X.screens[i].root, &dw1, &dw2, 
		&wins, &nwins);
	LOG_XDEBUG("%d windows\n", nwins);
	for (j = 0; j < nwins; j++)
		setup_each_client(i, j, wins);
	XFree(wins);
}

static void
setup_display_environment_variable(int i)
{
	char *ds, *colon, *dot;

	ds = DisplayString(arwm.X.dpy);
	/* set up DISPLAY environment variable to use */
	colon = strrchr(ds, ':');
	if (colon && arwm.X.num_screens > 1)
	{
		arwm.X.screens[i].display = malloc(14 + strlen(ds));
		strcpy(arwm.X.screens[i].display, "DISPLAY=");
		strcat(arwm.X.screens[i].display, ds);
		colon = strrchr(arwm.X.screens[i].display, ':');
		dot = strchr(colon, '.');
		if (!dot)
			dot = colon + strlen(colon);
		snprintf(dot, 5, ".%d", i);
	}
	else
		arwm.X.screens[i].display = NULL;
}

static void
setup_screen_elements(int i)
{
	arwm.X.screens[i].screen = i;
	arwm.X.screens[i].root = RootWindow(arwm.X.dpy, i);
	arwm.X.screens[i].vdesk = KEY_TO_VDESK(XK_1);
}

static void
setup_display_per_screen(int i)
{
	setup_display_environment_variable(i);
	setup_screen_elements(i);
	setup_gc(i);
	allocate_colors(i);
	setup_event_listeners(i);
	grab_keys_for_screen(&arwm.X.screens[i]);
	/* scan all the windows on this screen */
	LOG_XDEBUG("main:XQueryTree(); ");
	setup_clients(i);
}

static void
setup_shape(void)
{
	int e_dummy;

	arwm.X.have_shape = XShapeQueryExtension(arwm.X.dpy, 
		&arwm.X.shape_event, &e_dummy);
}

static void
setup_screens(void)
{
	/* now set up each screen in turn */
	arwm.X.num_screens = ScreenCount(arwm.X.dpy);
	if (arwm.X.num_screens < 0)
	{
		LOG_ERROR("Can't count screens\n");
		exit(1);
	}
	arwm.X.screens = malloc(arwm.X.num_screens * sizeof(ScreenInfo));
	{
		/* used in scanning windows (XQueryTree) */
		int i;

		for (i = arwm.X.num_screens; i--;)
			setup_display_per_screen(i);
	}
}

static void 
setup_display(void)
{
	arwm.X.dpy = XOpenDisplay(arwm_opt_display);
	if (!arwm.X.dpy)
	{
		LOG_ERROR("can't open display %s\n", arwm_opt_display);
		exit(1);
	}
	XSetErrorHandler(handle_xerror);
	setup_atoms();
	setup_fonts();
	setup_cursors();
	setup_numlock_key_modifier();
	setup_shape();
	setup_screens();
}


int 
main(int argc, char **argv)
{
	initialize_ARWMEnvironment();
	parse_command_line_args(argc, argv);
	setup_display();
	setup_signal_handlers();
	event_main_loop();

	return 1;
}


