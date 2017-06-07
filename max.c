// Copyright 2017, Jeffrey E. Bedard
#include "max.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "font.h"
#include "move_resize.h"
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
	jbwm_ewmh_remove_state(c->display, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ));
}
void jbwm_set_not_horz(struct JBWMClient * restrict c)
{
	if (c->opt.max_horz)
		set_not_horz(c);
	jbwm_move_resize(c);
}
static void set_horz(struct JBWMClient * restrict c,
	const uint16_t display_width)
{
	c->opt.max_horz = true;
	c->old_size.x = c->size.x;
	c->old_size.width = c->size.width;
	c->size.x = 0;
	c->size.width = display_width;
}
void jbwm_set_horz(struct JBWMClient * restrict c)
{
	if (c->opt.max_horz)
		return;
	jbwm_ewmh_add_state(c->display, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ));
	set_horz(c, DisplayWidth(c->display, jbwm_get_screen(c)->id));
	c->size.width -= c->border << 1;
	jbwm_move_resize(c);
}
static void set_not_vert(struct JBWMClient * restrict c)
{
	c->opt.max_vert = false;
	c->size.y = c->old_size.y;
	c->size.height = c->old_size.height;
	jbwm_ewmh_remove_state(c->display, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_MAXIMIZED_VERT));
}
void jbwm_set_not_vert(struct JBWMClient * restrict c)
{
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
	c->size.height = DisplayHeight(c->display, jbwm_get_screen(c)->id);
	jbwm_ewmh_add_state(c->display, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_MAXIMIZED_VERT));
}
void jbwm_set_vert(struct JBWMClient * restrict c)
{
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
	c->size = c->before_fullscreen;
	Display * d = c->display;
	XSetWindowBorderWidth(d, c->parent, c->border);
	jbwm_ewmh_remove_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_FULLSCREEN));
	jbwm_update_title_bar(c);
}
void jbwm_set_not_fullscreen(struct JBWMClient * restrict c)
{
	if(c->opt.fullscreen)
		set_not_fullscreen(c);
	jbwm_move_resize(c);
}
static void set_fullscreen(struct JBWMClient * restrict c)
{
	c->opt.fullscreen = true;
	c->before_fullscreen = c->size;
	c->size.x = c->size.y = 0;
	const uint8_t id = jbwm_get_screen(c)->id;
	Display * d = c->display;
	c->size.width = DisplayWidth(d, id);
	c->size.height = DisplayHeight(d, id);
	XSetWindowBorderWidth(d, c->parent, 0);
	jbwm_ewmh_add_state(d, c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_FULLSCREEN));
	jbwm_update_title_bar(c);
}
void jbwm_set_fullscreen(struct JBWMClient * restrict c)
{
	if (c->opt.fullscreen || c->opt.shaded || c->opt.no_max)
		return;
	set_fullscreen(c);
	jbwm_move_resize(c);
}
