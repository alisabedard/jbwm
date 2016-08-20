// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "button_event.h"

#include "client.h"
#include "JBWMEnv.h"
#include "screen.h"
#include "titlebar.h"

#ifdef USE_TBAR
__attribute__((nonnull))
static void titlebar_event(struct JBWMClient * restrict c, const uint16_t x)
{
	const uint16_t w = c->size.width;
	if (!c->opt.no_close && (x < TDIM)) {
		/* This fixes a bug where deleting a shaded window will cause
		   the parent window to stick around as a ghost window. */
		if (c->opt.shaded) shade(c);
		send_wm_delete(c);
	} else if (!c->opt.no_resize && (x > w - TDIM)) jbwm_drag(c, true);
	else if (!c->opt.no_min && (x > w - (TDIM<<1))) shade(c);
	else jbwm_drag(c, false);
}
#endif//USE_TBAR

void jbwm_handle_button_event(XButtonEvent * restrict e, struct JBWMClient * restrict c)
{
	switch (e->button) {
	case Button1:
#ifdef USE_TBAR
		if(e->window==c->titlebar)
			titlebar_event(c, e->x);
		else
#endif//USE_TBAR
			jbwm_drag(c, false);
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
