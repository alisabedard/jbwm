// Copyright 2016, Jeffrey E. Bedard

#include "max.h"

#include "ewmh.h"
#include "ewmh_state.h"
#include "JBWMEnv.h"
#include "log.h"
#include "screen.h"
#include "titlebar.h"

void jbwm_set_not_horz(struct JBWMClient * restrict c)
{
	LOG("jbwm_set_not_horz");
	if (!c->opt.max_horz) return;
	c->opt.max_horz = false;
	c->size.x = c->old_size.x;
	c->size.width = c->old_size.width;
	ewmh_remove_state(c->window, ewmh[WM_STATE_MAXIMIZED_HORZ]);
	jbwm_move_resize(c);
}

void jbwm_set_horz(struct JBWMClient * restrict c)
{
	LOG("jbwm_set_horz");
	if (c->opt.max_horz) return;
	c->old_size.x = c->size.x;
	c->old_size.width = c->size.width;
	c->size.x = 0;
	c->size.width = c->screen->size.w;
	ewmh_add_state(c->window, ewmh[WM_STATE_MAXIMIZED_HORZ]);
	c->opt.max_horz = true;
	// Offset if not fullscreen
	if (!c->opt.fullscreen)
		c->size.width -= c->border<<1;
	jbwm_move_resize(c);
}

void jbwm_set_not_vert(struct JBWMClient * restrict c)
{
	LOG("jbwm_set_not_vert");
	if (c->opt.max_vert && !c->opt.shaded) {
		c->opt.max_vert = false;
		c->size.y = c->old_size.y;
		c->size.height = c->old_size.height;
		ewmh_remove_state(c->window,
			ewmh[WM_STATE_MAXIMIZED_VERT]);
	}
	jbwm_move_resize(c);
}

void jbwm_set_vert(struct JBWMClient * restrict c)
{
	LOG("jbwm_set_vert");
	if (c->opt.max_vert || c->opt.shaded) return;
	c->old_size.y = c->size.y;
	c->old_size.height = c->size.height;
	c->size.y = 0;
	c->size.height = c->screen->size.h;
	ewmh_add_state(c->window, ewmh[WM_STATE_MAXIMIZED_VERT]);
	c->opt.max_vert = true;
	// Offset the titlebar if not fullscreen
	if (!c->opt.fullscreen) {
		c->size.y += TDIM+c->border;
		c->size.height -= TDIM+(c->border<<2);
	}
	jbwm_move_resize(c);
}

void jbwm_set_not_fullscreen(struct JBWMClient * restrict c)
{
	LOG("jbwm_set_not_fullscreen");
	if(!c->opt.fullscreen) return;
	c->opt.fullscreen = false; // Reflects desired status
	jbwm_set_not_horz(c);
	jbwm_set_not_vert(c);
	XSetWindowBorderWidth(jbwm.d, c->parent, c->border);
	ewmh_remove_state(c->window, ewmh[WM_STATE_FULLSCREEN]);
	jbwm_update_titlebar(c);
}

void jbwm_set_fullscreen(struct JBWMClient * restrict c)
{
	LOG("jbwm_set_fullscreen");
	if(c->opt.fullscreen || c->opt.shaded || c->opt.no_max)
		  return;
	/* The following checks remove conflicts between fullscreen
	   mode and setd modes.  */
	if(c->opt.max_horz)
		jbwm_set_not_horz(c);
	if(c->opt.max_vert)
		jbwm_set_not_vert(c);
	c->opt.fullscreen = true; // Reflect desired status
	jbwm_set_horz(c);
	jbwm_set_vert(c);
	XSetWindowBorderWidth(jbwm.d, c->parent, 0);
	ewmh_add_state(c->window, ewmh[WM_STATE_FULLSCREEN]);
	jbwm_update_titlebar(c);
}

