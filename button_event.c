// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "button_event.h"

#include "client.h"
#include "JBWMEnv.h"
#include "log.h"
#include "screen.h"
#include "titlebar.h"

#ifdef USE_TBAR
static void handle_titlebar_button(XButtonEvent * restrict e,
	struct JBWMClient * restrict c)
{
	LOG("e->window: %d, c->titlebar: %d, e->subwindow: %d",
		(int)e->window, (int)c->tb.win, (int)e->subwindow);
	if (e->subwindow == c->tb.close)
		jbwm_send_wm_delete(c);
	else if (e->subwindow == c->tb.resize)
		jbwm_drag(c, !c->opt.no_resize);
	else if (e->subwindow == c->tb.shade && !c->opt.no_min)
		jbwm_toggle_shade(c);
	else if (e->subwindow == c->tb.stick)
		jbwm_toggle_sticky(c);
	else
		jbwm_drag(c, false);
}
#else//!USE_TBAR
#define handle_titlebar_button(e, c) jbwm_drag(c, false)
#endif//USE_TBAR

void jbwm_handle_button_event(XButtonEvent * restrict e,
	struct JBWMClient * restrict c)
{
	LOG("jbwm_handle_button_event");
	switch (e->button) {
	case Button1:
		handle_titlebar_button(e, c);
		break;
	case Button2:
		XLowerWindow(jbwm.d, c->parent);
		break;
	case Button3:
		/* Resize operations more useful here,
		   rather than for third button, for laptop
		   users especially, where it is difficult
		   to register a middle button press, even
		   with X Emulate3Buttons enabled.  */
		jbwm_drag(c, !c->opt.shaded);
		break;
	}
}
