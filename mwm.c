// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "mwm.h"
#include "JBWMClient.h"
#include "log.h"
#include "mwmproto.h"
#include "util.h"
//#define JBWM_ENABLE_NO_RESIZE_HINT
#define JBWM_ENABLE_TITLE_BAR_HINT
static void process_flags(struct JBWMClient * restrict c)
{
	struct JBWMClientOptions * o = &c->opt;
	if (o->tearoff) {
		o->no_border = o->no_resize = o->no_min = o->no_max
			= o->no_title_bar = true;
	}
	c->border = o->no_border ? 0 : 1;
}
static void do_functions(struct JBWMClientOptions * o,
	const enum MwmFunctions f)
{
	/* Qt 4 and 5 exhibit bad behavior if these three checks are
	   not disabled.  Since Qt 4 and 5 applications are much more
	   common than Motif applications today, defer to behavior
	   which works with both.  */
#ifdef JBWM_NO_QT_FIX
#ifdef JBWM_ENABLE_NO_RESIZE_HINT
	if (!(f & MWM_FUNC_RESIZE))
		o->no_resize = true;
#endif//JBWM_ENABLE_NO_RESIZE_HINT
	if (!(f & MWM_FUNC_MINIMIZE))
		o->no_min = true;
	if (!(f & MWM_FUNC_CLOSE))
		o->no_close = true;
#endif//JBWM_NO_QT_FIX
#ifdef ENABLE_NO_MAX
	if (!(f & MWM_FUNC_MAXIMIZE))
		o->no_max = true;
#endif//ENABLE_NO_MAX
	if (!(f & MWM_FUNC_MOVE))
		o->no_move = true;
	JBWM_LOG("MWM_HINTS_FUNCTIONS\topts: %d, %d, %d, %d, %d",
		o->no_resize, o->no_close, o->no_min, o->no_max,
		o->no_move);
}
static void do_decorations(struct JBWMClientOptions * o,
	const enum MwmDecor f)
{
	o->no_border = !(f & MWM_DECOR_BORDER);
#ifdef ENABLE_NO_RESIZE
	if (f & MWM_DECOR_RESIZEH) {
		JBWM_LOG("decor resize");
		o->no_resize = false;
	}
#endif//ENABLE_NO_RESIZE
	if (f & MWM_DECOR_MENU) {
		JBWM_LOG("decor close");
		o->no_close = false;
	}
	if (f & MWM_DECOR_MINIMIZE) {
		JBWM_LOG("decor min");
		o->no_min = false;
	}
#ifdef JBWM_ENABLE_TITLE_BAR_HINT
	if (f & MWM_DECOR_TITLE) {
		JBWM_LOG("decor title_bar");
		o->no_title_bar = false;
	} else {
		JBWM_LOG("decor no_title_bar");
		o->no_title_bar = true;
	}
#endif//JBWM_ENABLE_TITLE_BAR_HINT
}
static Atom get_mwm_hints_atom(Display * d)
{
	static Atom a;
	return a ? a : (a = XInternAtom(d, "_MOTIF_WM_HINTS", false));
}
void jbwm_handle_mwm_hints(Display * d, struct JBWMClient * restrict c)
{
	const Atom mwm_hints = get_mwm_hints_atom(d);
	struct JBWMMwm * m = jbwm_get_property(d, c->window,
		mwm_hints, &(uint16_t){0});
	if(!m)
		return;
	if (!((c->opt.tearoff = m->flags & MWM_HINTS_STATUS && m->status &
		MWM_TEAROFF_WINDOW))) {
		// skip the following if tear-off window
		if (m->flags & MWM_HINTS_FUNCTIONS
			&& !(m->functions & MWM_FUNC_ALL))
			do_functions(&c->opt, m->functions);
		if (m->flags & MWM_HINTS_DECORATIONS
			&& !(m->decor & MWM_DECOR_ALL))
			do_decorations(&c->opt, m->decor);
	}
	XFree(m);
	process_flags(c);
}
