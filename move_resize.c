// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "move_resize.h"
#include "JBWMClient.h"
#include "display.h"
#include "font.h"
#include "log.h"
#include "mwm.h"
#include "shape.h"
#include "title_bar.h"
__attribute__((nonnull))
static void jbwm_configure_client(Display * d,
	struct JBWMClient * restrict c)
{
	const Window w = c->window;
	struct JBWMRectangle * restrict g = &c->size;
	XSendEvent(d, w, true, StructureNotifyMask, (XEvent
		*) &(XConfigureEvent){.x = g->x, .y = g->y, .width = g->width,
		.height = g->height, .type = ConfigureNotify, .event = w,
		.window = w, .above = c->parent, .override_redirect = true,
		.border_width = c->border});
}
void jbwm_move_resize(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_move_resize");
	struct JBWMClientOptions * restrict o = &c->opt;
	const uint8_t offset = o->no_title_bar || o->fullscreen
		? 0 : jbwm_get_font_height();
	if(offset) { // Leave braces in case title bar support was disabled.
		jbwm_handle_mwm_hints(c);
		jbwm_update_title_bar(c);
	} // Skip shaped and fullscreen clients.
	Display * d = jbwm_get_display();
	{ //  * g scope
		struct JBWMRectangle * restrict g = &c->size;
		XMoveResizeWindow(d, c->parent, g->x, g->y - offset,
			g->width, g->height + offset);
		XMoveResizeWindow(d, c->window, 0, offset,
			g->width, g->height);
	}
	jbwm_set_shape(c);
	jbwm_configure_client(d, c);
}

