// Copyright 2016, Jeffrey E. Bedard
#include "max.h"
#include "JBWMClient.h"
#include "JBWMScreen.h"
#include "display.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "font.h"
#include "log.h"
#include "screen.h"
#include "title_bar.h"
/* In this file, the static companion functions perform the requested option
   directly, while the global functions call the corresponding static
   function and perform sanity checks and adjustments.  */
static void set_not_horz(struct JBWMClient * restrict c)
{
	c->opt.max_horz = false;
	c->size.x = c->old_size.x;
	c->size.width = c->old_size.width;
	jbwm_ewmh_remove_state(jbwm_get_display(), c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ));
}
void jbwm_set_not_horz(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_set_not_horz");
	if (c->opt.max_horz)
		set_not_horz(c);
	jbwm_move_resize(c);
}
static void set_horz(struct JBWMClient * restrict c)
{
	c->opt.max_horz = true;
	c->old_size.x = c->size.x;
	c->old_size.width = c->size.width;
	c->size.x = 0;
	c->size.width = jbwm_get_screen(c)->size.width;
}
void jbwm_set_horz(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_set_horz");
	if (c->opt.max_horz) return;
	jbwm_ewmh_add_state(jbwm_get_display(), c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ));
	set_horz(c);
	c->size.width -= c->border << 1;
	jbwm_move_resize(c);
}
static void set_not_vert(struct JBWMClient * restrict c)
{
	c->opt.max_vert = false;
	c->size.y = c->old_size.y;
	c->size.height = c->old_size.height;
	jbwm_ewmh_remove_state(jbwm_get_display(), c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_MAXIMIZED_VERT));
}
void jbwm_set_not_vert(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_set_not_vert");
	if (c->opt.max_vert && !c->opt.shaded)
		set_not_vert(c);
	jbwm_move_resize(c);
}
static void set_vert(struct JBWMClient * restrict c)
{
	c->opt.max_vert = true;
	c->old_size.y = c->size.y;
	c->old_size.height = c->size.height;
	c->size.y = 0;
	c->size.height = jbwm_get_screen(c)->size.height;
	jbwm_ewmh_add_state(jbwm_get_display(), c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_MAXIMIZED_VERT));
}
void jbwm_set_vert(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_set_vert");
	if (c->opt.max_vert || c->opt.shaded)
		return;
	set_vert(c);
	// Offset to hide borders:
	c->size.height -= c->border << 1;
	if (!c->opt.no_title_bar) {
		c->size.y += jbwm_get_font_height() + c->border;
		c->size.height -= jbwm_get_font_height() + (c->border << 1);
	}
	jbwm_move_resize(c);
}
static void set_not_fullscreen(struct JBWMClient * restrict c)
{
	c->opt.fullscreen = false;
	set_not_horz(c);
	set_not_vert(c);
	Display * d = jbwm_get_display();
	XSetWindowBorderWidth(d, c->parent, c->border);
	jbwm_ewmh_remove_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_FULLSCREEN));
	jbwm_update_title_bar(c);
}
void jbwm_set_not_fullscreen(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_set_not_fullscreen");
	if(c->opt.fullscreen)
		set_not_fullscreen(c);
	jbwm_move_resize(c);
}
static void set_fullscreen(struct JBWMClient * restrict c)
{
	c->opt.fullscreen = true;
	/* If the window is already maximized in any direction,
	   restore its original size before setting fullscreen.
	   Without these checks, the window will loose its
	   original size after being set fullscreen.  */
	if (c->opt.max_horz)
		set_not_horz(c);
	if (c->opt.max_vert)
		set_not_vert(c);
	set_horz(c);
	set_vert(c);
	Display * d = jbwm_get_display();
	XSetWindowBorderWidth(d, c->parent, 0);
	jbwm_ewmh_add_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_FULLSCREEN));
	jbwm_update_title_bar(c);
}
void jbwm_set_fullscreen(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_set_fullscreen");
	if (c->opt.fullscreen || c->opt.shaded || c->opt.no_max)
		return;
	set_fullscreen(c);
	jbwm_move_resize(c);
}
