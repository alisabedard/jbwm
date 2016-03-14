// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com>
#include "jbwm.h"

// flags:
#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_HINTS_INPUT_MODE    (1L << 2)
#define MWM_HINTS_STATUS        (1L << 3)

// functions:
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

// decor:
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)

// input_mode:
#define MWM_INPUT_MODELESS                      0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL     1
#define MWM_INPUT_SYSTEM_MODAL                  2
#define MWM_INPUT_FULL_APPLICATION_MODAL        3

// status:
#define MWM_TEAROFF_WINDOW 1

static void process_flags(Client * c)
{
	if (c->flags & JB_NO_BORDER)
		c->border = 0;

	if (c->flags & JB_TEAROFF) {
		c->border = 0;
		c->flags |= JB_NO_RESIZE | JB_NO_MIN | JB_NO_MAX;
	}
}

static void mwm_hints_decor(Client * c, unsigned long f)
{
	LOG("MWM_HINTS_DECORATIONS");

	if (f & MWM_DECOR_ALL)
		return;

	if (!(f & MWM_DECOR_BORDER))
		c->flags |= JB_NO_BORDER;

	if (!(f & MWM_DECOR_RESIZEH))
		c->flags |= JB_NO_RESIZE_DECOR;

	if (!(f & MWM_DECOR_TITLE))
		c->flags |= JB_NO_TB;

	if (!(f & MWM_DECOR_MENU))
		c->flags |= JB_NO_CLOSE_DECOR;

	if (!(f & MWM_DECOR_MINIMIZE))
		c->flags |= JB_NO_MIN_DECOR;

	if (!(f & MWM_DECOR_MAXIMIZE))
		c->flags |= JB_NO_MAX_DECOR;
}

static void mwm_hints_func(Client * c, unsigned long f)
{
	LOG("MWM_HINTS_FUNCTIONS");

	if (f & MWM_FUNC_ALL)
		return;

	if (!(f & MWM_FUNC_RESIZE))
		c->flags |= JB_NO_RESIZE | JB_NO_RESIZE_DECOR;

	if (!(f & MWM_FUNC_MOVE))
		c->flags |= JB_NO_MOVE;

	if (!(f & MWM_FUNC_CLOSE))
		c->flags |= JB_NO_CLOSE | JB_NO_CLOSE_DECOR;

	if (!(f & MWM_FUNC_MINIMIZE))
		c->flags |= JB_NO_MIN | JB_NO_MIN_DECOR;

	if (!(f & MWM_FUNC_MAXIMIZE))
		c->flags |= JB_NO_MAX | JB_NO_MAX_DECOR;
}

void handle_mwm_hints(Client * c)
{
	static Atom mwm_hints;
	assert(c);
	if(!mwm_hints)
		  mwm_hints=XA("_MOTIF_WM_HINTS");
	unsigned long n;
	struct {
		unsigned long flags, functions, decor, input_mode, status;
	} *m = get_property(c->window, mwm_hints, mwm_hints, &n);

	if (!m)
		return;

	if (m->flags & MWM_HINTS_FUNCTIONS)
		mwm_hints_func(c, m->functions);

	if (m->flags & MWM_HINTS_DECORATIONS)
		mwm_hints_decor(c, m->decor);

	if (m->flags & MWM_HINTS_STATUS) {
		LOG("MWM_HINTS_STATUS");

		if (m->status & MWM_TEAROFF_WINDOW)
			c->flags |= JB_TEAROFF;
	}

	if (m->flags & MWM_HINTS_INPUT_MODE) {
		LOG("MWM_HINTS_INPUT_MODE");

		if (m->input_mode)
			c->flags |= JB_MODAL;
	}

	XFree(m);
	process_flags(c);
}
