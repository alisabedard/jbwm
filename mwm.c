// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "mwm.h"

#include "JBWMEnv.h"
#include "log.h"
#include "util.h"

#include <X11/Xatom.h>

// These are MWM-specific hints
enum {
// flags:
	MWM_HINTS_FUNCTIONS = (1L << 0), MWM_HINTS_DECORATIONS = (1L << 1),
	MWM_HINTS_INPUT_MODE = (1L << 2), MWM_HINTS_STATUS = (1L << 3),
// functions:
	MWM_FUNC_ALL = (1L << 0), MWM_FUNC_RESIZE = (1L << 1),
	MWM_FUNC_MOVE = (1L << 2), MWM_FUNC_MINIMIZE = (1L << 3),
	MWM_FUNC_MAXIMIZE = (1L << 4), MWM_FUNC_CLOSE = (1L << 5),
// decor:
	MWM_DECOR_ALL = (1L << 0), MWM_DECOR_BORDER = (1L << 1),
	MWM_DECOR_RESIZEH = (1L << 2), MWM_DECOR_TITLE = (1L << 3),
	MWM_DECOR_MENU = (1L << 4), MWM_DECOR_MINIMIZE = (1L << 5),
	MWM_DECOR_MAXIMIZE = (1L << 6)
};

// status:
enum { MWM_TEAROFF_WINDOW = 1 };

static void process_flags(struct JBWMClient * c)
{
	if (c->opt.tearoff) {
		c->opt.no_border = c->opt.no_resize = c->opt.no_min
			= c->opt.no_max = true;
	}
	c->border=!c->opt.no_border;
}

struct JBWMMWM { // paraphrased from MwmUtil.h
	uint32_t flags, functions, decor, input_mode, status;
};

static bool do_status(struct JBWMClient * restrict c,
	struct JBWMMWM * restrict m)
{
	if (m->flags & MWM_HINTS_STATUS) {
		LOG("MWM_HINTS_STATUS");
		c->opt.tearoff = m->status & MWM_TEAROFF_WINDOW;
		return true;
	}
	return false;
}

static void do_functions(struct JBWMClient * restrict c,
	struct JBWMMWM * restrict m)
{
	if (m->flags & MWM_HINTS_FUNCTIONS
		&& !(m->functions&MWM_FUNC_ALL)) {
		LOG("MWM_HINTS_FUNCTIONS");
		c->opt.no_resize=!(m->functions&MWM_FUNC_RESIZE);
		c->opt.no_close=!(m->functions&MWM_FUNC_CLOSE);
		c->opt.no_min=!(m->functions&MWM_FUNC_MINIMIZE);
		c->opt.no_max=!(m->functions&MWM_FUNC_MAXIMIZE);
		c->opt.no_move=!(m->functions&MWM_FUNC_MOVE);
		LOG("\topts: %d, %d, %d, %d, %d",
			c->opt.no_resize,
			c->opt.no_close,
			c->opt.no_min,
			c->opt.no_max,
			c->opt.no_move);
	}

}

static void do_decorations(struct JBWMClient * restrict c,
	struct JBWMMWM * restrict m)
{
	if (m->flags & MWM_HINTS_DECORATIONS
		&& !(m->decor&MWM_DECOR_ALL)) {
		LOG("MWM_HINTS_DECORATIONS");
		c->opt.no_border=!(m->decor&MWM_DECOR_BORDER);
		c->opt.no_resize_decor=!(m->decor&MWM_DECOR_RESIZEH);
		c->opt.no_titlebar=!(m->decor&MWM_DECOR_TITLE);
		c->opt.no_close_decor=!(m->decor&MWM_DECOR_MENU);
		c->opt.no_min_decor=!(m->decor&MWM_DECOR_MINIMIZE);
		//c->opt.no_max_decor=!(m->decor&MWM_DECOR_MAXIMIZE);
		LOG("\topts: %d, %d, %d, %d",
			c->opt.no_resize_decor,
			c->opt.no_titlebar,
			c->opt.no_close_decor,
			c->opt.no_min_decor);
	}

}

static Atom get_mwm_hints_atom(void)
{
	static Atom a;
	return a ? a : (a = XInternAtom(jbwm.d, "_MOTIF_WM_HINTS", false));
}

void handle_mwm_hints(struct JBWMClient * c)
{
	const Atom mwm_hints = get_mwm_hints_atom();
	struct JBWMMWM * m = jbwm_get_property(c->window,
		mwm_hints, &(uint16_t){0});
	if(!m)
		return;
	if (do_status(c, m))
		goto mwm_end;
	do_functions(c, m);
	do_decorations(c, m);


mwm_end:
	XFree(m);
	process_flags(c);
}

