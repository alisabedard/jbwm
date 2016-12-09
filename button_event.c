// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#undef DEBUG
#include "button_event.h"
#include "client.h"
#include "log.h"
#include "screen.h"
#include "title_bar.h"
#include "wm_state.h"
#ifdef JBWM_USE_TITLE_BAR
static void handle_title_bar_button(XButtonEvent * restrict e,
	struct JBWMClient * restrict c)
{
	JBWM_LOG("e->window: %d, c->title_bar: %d, e->subwindow: %d",
		(int)e->window, (int)c->tb.win, (int)e->subwindow);
	struct JBWMClientOptions * o = &c->opt;
	if (!e->subwindow)
		jbwm_drag(c, false);
	else if (e->subwindow == c->tb.close && !o->no_close)
		jbwm_send_wm_delete(c);
	else if (e->subwindow == c->tb.resize && !o->no_resize)
		jbwm_drag(c, !c->opt.no_resize);
	else if (e->subwindow == c->tb.shade && !c->opt.no_min)
		jbwm_toggle_shade(c);
	else if (e->subwindow == c->tb.stick)
		jbwm_toggle_sticky(c);
	else
		jbwm_drag(c, false);
}
#else//!JBWM_USE_TITLE_BAR
#define handle_title_bar_button(e, c) jbwm_drag(c, false)
#endif//JBWM_USE_TITLE_BAR
__attribute__((nonnull))
void jbwm_handle_button_event(XButtonEvent * restrict e,
	struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_handle_button_event");
	const bool fs = c->opt.fullscreen;
	Display * d = e->display;
	switch (e->button) {
	case Button1:
		if (fs)
			XRaiseWindow(c->d, c->parent);
		else
			handle_title_bar_button(e, c);
		break;
	case Button2:
		XLowerWindow(d, c->parent);
		break;
	case Button3:
		/* Resize operations more useful here,
		   rather than for third button, for laptop
		   users especially, where it is difficult
		   to register a middle button press, even
		   with X Emulate3Buttons enabled.  */
		if (fs)
			XLowerWindow(c->d, c->parent);
		else
			jbwm_drag(c, !c->opt.shaded);
		break;
	}
}
