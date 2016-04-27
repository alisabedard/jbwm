// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "mwm.h"


#include "jbwmenv.h"
#include "log.h"
#include "util.h"

#include <assert.h>
#include <X11/Xatom.h>

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

// input_mode:
enum {
	MWM_INPUT_MODELESS,
	MWM_INPUT_PRIMARY_APPLICATION_MODAL,
	MWM_INPUT_SYSTEM_MODAL,
	MWM_INPUT_FULL_APPLICATION_MODAL
};

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

__attribute__((const))
static inline uint32_t hint(const uint32_t hint_flags,
	const uint32_t hint, const uint32_t assoc_flag,
	const uint32_t client_flags) // client_flags last for chained calling
{
	/* flags stored in Client.flags indicate negative options,
	   so invert test logic.  */
	if(hint_flags & hint)
		  return client_flags;
	return client_flags | assoc_flag;
}

__attribute__((const))
static uint32_t mwm_hints_decor(const uint32_t client_flags, const unsigned long f)
{
	if (f & MWM_DECOR_ALL)
		  return client_flags;
	return hint(f, MWM_DECOR_BORDER, JB_NO_RESIZE_DECOR,
		hint(f, MWM_DECOR_RESIZEH, JB_NO_RESIZE_DECOR,
		hint(f, MWM_DECOR_TITLE, JB_NO_TB,
		hint(f, MWM_DECOR_MENU, JB_NO_CLOSE_DECOR,
		hint(f, MWM_DECOR_MINIMIZE, JB_NO_MIN_DECOR,
		hint(f, MWM_DECOR_MAXIMIZE, JB_NO_MAX_DECOR,
		client_flags)))))); // <== input
}

__attribute__((const))
static uint32_t mwm_hints_func(const uint32_t client_flags, const unsigned long f)
{
	if (f & MWM_FUNC_ALL)
		  return client_flags;
	return hint(f, MWM_FUNC_RESIZE, JB_NO_RESIZE_DECOR|JB_NO_RESIZE_DECOR,
		hint(f, MWM_FUNC_CLOSE, JB_NO_CLOSE_DECOR|JB_NO_CLOSE,
		hint(f, MWM_FUNC_MINIMIZE, JB_NO_MIN_DECOR|JB_NO_MIN,
		hint(f, MWM_FUNC_MAXIMIZE, JB_NO_MAX_DECOR|JB_NO_MAX,
		hint(f, MWM_FUNC_MOVE, JB_NO_MOVE,
		client_flags))))); // <== input
}

void handle_mwm_hints(Client * c)
{
	static Atom mwm_hints;
	assert(c);
	if(!mwm_hints)
		  mwm_hints=XInternAtom(jbwm.dpy, "_MOTIF_WM_HINTS", false);
	unsigned long n;
	struct {
		unsigned long flags, functions, decor, input_mode, status;
	} *m = get_property(c->window, mwm_hints, &n);

	if (!m) return;

	if (m->flags & MWM_HINTS_FUNCTIONS)
		  c->flags=mwm_hints_func(c->flags, m->functions);

	if (m->flags & MWM_HINTS_DECORATIONS)
		  c->flags=mwm_hints_decor(c->flags, m->decor);

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
