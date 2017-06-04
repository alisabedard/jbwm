// Copyright 2017, Jeffrey E. Bedard
#include "geometry.h"
#include "JBWMClient.h"
#include "JBWMScreen.h"
#include "JBWMSize.h"
#include "log.h"
#include "macros.h"
#include "screen.h"
static long handle_wm_normal_hints(Display * d, const Window win,
	const struct JBWMRectangle * restrict attribute_geometry,
	struct JBWMRectangle * restrict geometry_return)
{
	/* Though these statements may be combined, writing the following
	 * assignment this way ensures the conditional is only evaluated once.
	 * */
	XSizeHints h;
	if (XGetWMNormalHints(d, win, &h, &(long){0})
		&& (h.flags & USSize)) {
		// if size hints provided, use them
		geometry_return->width = JB_MAX(h.width, h.min_width);
		geometry_return->height = JB_MAX(h.height, h.min_height);
	} else { // use existing window attributes
		geometry_return->width = attribute_geometry->width;
		geometry_return->height = attribute_geometry->height;
	}
	return h.flags;
}
static void check_dimensions(struct JBWMRectangle * restrict g,
	const struct JBWMSize screen)
{
	g->width = JB_MIN(g->width, screen.width);
	g->height = JB_MIN(g->height, screen.height);
}
__attribute__((const))
static int16_t get_center(const uint16_t wh, const uint16_t swh)
{
	return (swh - wh) >> 1;
}
static void center(struct JBWMRectangle * restrict g,
	const struct JBWMSize s)
{
	g->x = get_center(g->width, s.width);
	g->y = get_center(g->height, s.height);
}
// returns true if window is viewable
static bool get_window_attributes(Display * d, struct JBWMClient * restrict c,
	struct JBWMRectangle * restrict attribute_geometry)
{
	XWindowAttributes a;
	XGetWindowAttributes(d, c->window, &a);
	JBWM_LOG("XGetWindowAttributes() win: 0x%x,"
		"x: %d, y: %d, w: %d, h: %d",
		(int)c->window, a.x, a.y, a.width, a.height);
	c->cmap = a.colormap;
	*attribute_geometry = (struct JBWMRectangle){.x = a.x, .y = a.y,
		.width = a.width, .height = a.height};
	return a.map_state == IsViewable;
}
static void init_geometry_for_screen_size(Display * d, const Window window,
	struct JBWMRectangle * g, const struct JBWMRectangle * restrict
	attribute_geometry, const struct JBWMSize scr_sz)
{
	check_dimensions(g, scr_sz);
	const long flags = handle_wm_normal_hints(d, window,
		attribute_geometry, g);
	const bool user_specified_position = flags & USPosition;
	const bool nonzero_position = attribute_geometry->x ||
		attribute_geometry->y;
	if (user_specified_position && nonzero_position) {
		JBWM_LOG("\t\tPosition is set by hints.");
		g->x = attribute_geometry->x;
		g->y = attribute_geometry->y;
	} else // Position not specified
		center(g, scr_sz);
}
static void init_geometry_for_screen(Display * d,
	struct JBWMClient * c, struct JBWMRectangle * g,
	const struct JBWMRectangle * restrict attribute_geometry)
{
	struct JBWMScreen * s = jbwm_get_screen(c);
	if (s)
		init_geometry_for_screen_size(d, c->window, g,
			attribute_geometry, (struct JBWMSize){DisplayWidth(d,
			s->id), DisplayHeight(d, s->id)});
	else
		g->x = g->y = 0;
}
void jbwm_set_client_geometry(Display * d, struct JBWMClient * c)
{
	struct JBWMRectangle attribute_geometry;
	const bool viewable = get_window_attributes(d, c,
		&attribute_geometry);
	struct JBWMRectangle * g = &c->size;
	if (viewable) {
		/* Increment unmap event counter for the reparent event.  */
		++c->ignore_unmap;
		/* If the window is already on screen before the window
		   manager starts, simply save its geometry then return. */
		*g = attribute_geometry;
		return;
	}
	c->ignore_unmap += viewable ? 1 : 0;
	JBWM_LOG("\t\tVIEWABLE: %d", viewable);
	init_geometry_for_screen(d, c, g, &attribute_geometry);
	JBWM_LOG("init_geometry() win: 0x%x, x: %d, y: %d, w: %d, h: %d",
		(int)c->window, g->x, g->y, g->width, g->height);
}
