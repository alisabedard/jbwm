// Copyright 2016, Jeffrey E. Bedard

#include "max.h"

#include "ewmh.h"
#include "log.h"
#include "screen.h"
#include "titlebar.h"

void unset_horz(Client * restrict c)
{
	LOG("unset_horz");
	if (!c->opt.max_horz) return;
	c->opt.max_horz = false;
	c->size.x = c->old_size.x;
	c->size.width = c->old_size.width;
	ewmh_remove_state(c->window, ewmh[WM_STATE_MAXIMIZED_HORZ]);
}

void set_horz(Client * restrict c)
{
	LOG("set_horz");
	if (c->opt.max_horz) return;
	c->old_size.x = c->size.x;
	c->old_size.width = c->size.width;
	c->size.x = 0;
	c->size.width = c->screen->size.w;
	ewmh_add_state(c->window, ewmh[WM_STATE_MAXIMIZED_HORZ]);
	c->opt.max_horz = true;
	// Offset if not fullscreen
	if (!c->opt.fullscreen) {
		c->size.width -= c->border<<1;
	}
}

void unset_vert(Client * restrict c)
{
	LOG("unset_vert");
	if (c->opt.max_vert && !c->opt.shaded) {
		c->opt.max_vert = false;
		c->size.y = c->old_size.y;
		c->size.height = c->old_size.height;
		ewmh_remove_state(c->window,
			ewmh[WM_STATE_MAXIMIZED_VERT]);
	}
}

void set_vert(Client * restrict c)
{
	LOG("set_vert");
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
		moveresize(c);
	}
}

void unset_fullscreen(Client * restrict c)
{
	LOG("unset_fullscreen");
	if(!c->opt.fullscreen) return;
	c->opt.fullscreen = false; // Reflects desired status
	unset_horz(c);
	unset_vert(c);
	XSetWindowBorderWidth(jbwm.dpy, c->parent, c->border);
	ewmh_remove_state(c->window, ewmh[WM_STATE_FULLSCREEN]);
	update_titlebar(c);
}

void set_fullscreen(Client * restrict c)
{
	LOG("set_fullscreen");
	if(c->opt.fullscreen || c->opt.shaded)
		  return;
	/* The following checks remove conflicts between fullscreen
	   mode and setd modes.  */
	if(c->opt.max_horz) unset_horz(c);
	if(c->opt.max_vert) unset_vert(c);
	c->opt.fullscreen = true; // Reflect desired status
	set_horz(c);
	set_vert(c);
	XSetWindowBorderWidth(jbwm.dpy, c->parent, 0);
	ewmh_add_state(c->window, ewmh[WM_STATE_FULLSCREEN]);
	update_titlebar(c);
}


