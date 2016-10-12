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
enum MwmFlags {
// flags:
	MWM_HINTS_FUNCTIONS = (1L << 0), MWM_HINTS_DECORATIONS = (1L << 1),
	MWM_HINTS_INPUT_MODE = (1L << 2), MWM_HINTS_STATUS = (1L << 3)
};

enum MwmFunctions {
// functions:
	MWM_FUNC_ALL = (1L << 0), MWM_FUNC_RESIZE = (1L << 1),
	MWM_FUNC_MOVE = (1L << 2), MWM_FUNC_MINIMIZE = (1L << 3),
	MWM_FUNC_MAXIMIZE = (1L << 4), MWM_FUNC_CLOSE = (1L << 5),
};

enum MwmDecor {
// decor:
	MWM_DECOR_ALL = (1L << 0), MWM_DECOR_BORDER = (1L << 1),
	MWM_DECOR_RESIZEH = (1L << 2), MWM_DECOR_TITLE = (1L << 3),
	MWM_DECOR_MENU = (1L << 4), MWM_DECOR_MINIMIZE = (1L << 5),
	MWM_DECOR_MAXIMIZE = (1L << 6),
};

enum MwmStatus {
// status:
	MWM_TEAROFF_WINDOW = 1
};

struct JBWMMwm { // paraphrased from MwmUtil.h
	uint32_t flags, functions, decor, input_mode, status;
};

static void process_flags(struct JBWMClient * c)
{
	if (c->opt.tearoff) {
		c->opt.no_border = c->opt.no_resize = c->opt.no_min
			= c->opt.no_max = true;
	}
	c->border=!c->opt.no_border;
}

static void do_functions(struct JBWMClientOptions * restrict o,
	const enum MwmFunctions f)
{
		o->no_resize=!(f & MWM_FUNC_RESIZE);
		o->no_close=!(f & MWM_FUNC_CLOSE);
		o->no_min=!(f & MWM_FUNC_MINIMIZE);
		o->no_max=!(f & MWM_FUNC_MAXIMIZE);
		o->no_move=!(f & MWM_FUNC_MOVE);
		LOG("MWM_HINTS_FUNCTIONS\topts: %d, %d, %d, %d, %d",
			o->no_resize, o->no_close, o->no_min, o->no_max,
			o->no_move);
}

static void do_decorations(struct JBWMClientOptions * restrict o,
	const enum MwmDecor f)
{
		o->no_border=!(f & MWM_DECOR_BORDER);
		o->no_resize_decor=!(f & MWM_DECOR_RESIZEH);
		o->no_titlebar=!(f & MWM_DECOR_TITLE);
		o->no_close_decor=!(f & MWM_DECOR_MENU);
		o->no_min_decor=!(f & MWM_DECOR_MINIMIZE);
		//o->no_max_decor=!(f & MWM_DECOR_MAXIMIZE);
		LOG("MWM_HINTS_DECORATIONS\topts: %d, %d, %d, %d",
			o->no_resize_decor, o->no_titlebar, o->no_close_decor,
			o->no_min_decor);
}

static Atom get_mwm_hints_atom(void)
{
	static Atom a;
	return a ? a : (a = XInternAtom(jbwm.d, "_MOTIF_WM_HINTS", false));
}

void handle_mwm_hints(struct JBWMClient * c)
{
	const Atom mwm_hints = get_mwm_hints_atom();
	struct JBWMMwm * m = jbwm_get_property(c->window,
		mwm_hints, &(uint16_t){0});
	if(!m)
		return;
	if ((c->opt.tearoff = m->flags & MWM_HINTS_STATUS && m->status &
		MWM_TEAROFF_WINDOW))
		goto mwm_end; // skip the following if tear-off window
	if (m->flags & MWM_HINTS_FUNCTIONS && !(m->functions & MWM_FUNC_ALL))
		do_functions(&c->opt, m->functions);
	if (m->flags & MWM_HINTS_DECORATIONS && !(m->decor & MWM_DECOR_ALL))
		do_decorations(&c->opt, m->decor);
mwm_end:
	XFree(m);
	process_flags(c);
}

