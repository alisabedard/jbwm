// Copyright 2017, Jeffrey E. Bedard
#include "geometry.h"
#include "JBWMClient.h"
#include "JBWMScreen.h"
#include "JBWMSize.h"
#include "log.h"
#include "macros.h"
#include "screen.h"
struct GeometryData {
	Display * display;
	struct JBWMRectangle * restrict attribute;
	struct JBWMRectangle * restrict geometry;
	Window window;
};
static long handle_wm_normal_hints(struct GeometryData * restrict g)
{
	/* Though these statements may be combined, writing the following
	 * assignment this way ensures the conditional is only evaluated once.
	 * */
	XSizeHints h;
	if (XGetWMNormalHints(g->display, g->window, &h, &(long){0})
		&& (h.flags & USSize)) {
		// if size hints provided, use them
		g->geometry->width = JB_MAX(h.width, h.min_width);
		g->geometry->height = JB_MAX(h.height, h.min_height);
	} else // use existing window attributes
		*g->geometry = *g->attribute;
	return h.flags;
}
static void check_dimensions(struct JBWMRectangle * restrict g,
	const struct JBWMSize screen)
{
	g->width = JB_MIN(g->width, screen.width);
	g->height = JB_MIN(g->height, screen.height);
}
__attribute__((const))
static int16_t get_center(const int16_t window_size,
	const int16_t screen_size)
{
	return (screen_size - window_size) >> 1;
}
static void center(struct JBWMRectangle * restrict g,
	const struct JBWMSize s)
{
	g->x = get_center(g->width, s.width);
	g->y = get_center(g->height, s.height);
}
// returns true if window is viewable
static bool get_window_attributes(struct JBWMClient * restrict c,
	struct JBWMRectangle * restrict geometry_attribute)
{
	XWindowAttributes a;
	Display * d = c->display;
	XGetWindowAttributes(d, c->window, &a);
	JBWM_LOG("XGetWindowAttributes() win: 0x%x,"
		"x: %d, y: %d, w: %d, h: %d",
		(int)c->window, a.x, a.y, a.width, a.height);
	c->cmap = a.colormap;
	*geometry_attribute = (struct JBWMRectangle){.x = a.x, .y = a.y,
		.width = a.width, .height = a.height};
	return a.map_state == IsViewable;
}
static void init_geometry_for_screen_size(struct GeometryData * g,
	const struct JBWMSize screen_size)
{
	check_dimensions(g->geometry, screen_size);
	const long flags = handle_wm_normal_hints(g);
	const bool user_specified_position = flags & USPosition;
	const bool nonzero_position = g->attribute->x ||
		g->attribute->y;
	if (user_specified_position && nonzero_position) {
		JBWM_LOG("\t\tPosition is set by hints.");
		g->geometry->x = g->attribute->x;
		g->geometry->y = g->attribute->y;
	} else // Position not specified
		center(g->geometry, screen_size);
}
__attribute__((pure))
struct JBWMSize jbwm_get_display_size(Display * d, const uint8_t screen)
{
	return (struct JBWMSize) {DisplayWidth(d, screen),
		DisplayHeight(d, screen)};
}
static void init_geometry_for_screen(struct JBWMClient * c,
	struct JBWMRectangle * restrict geometry_attribute)
{
	struct JBWMScreen * s = jbwm_get_screen(c);
	if (s) {
		Display * d = c->display;
		init_geometry_for_screen_size(&(struct GeometryData){ .display
			= d, .attribute = geometry_attribute, .geometry =
			&c->size, .window = c->window},
			jbwm_get_display_size(d, s->id));
	} else
		c->size.x = c->size.y = 0;
}
void jbwm_set_client_geometry(struct JBWMClient * c)
{
	struct JBWMRectangle geometry_attribute;
	if (get_window_attributes(c, &geometry_attribute)) { // viewable
		/* Increment unmap event counter for the reparent event.  */
		++c->ignore_unmap;
		/* If the window is already on screen before the window
		   manager starts, simply save its geometry then return. */
		c->size = geometry_attribute;
	} else // not viewable yet
		init_geometry_for_screen(c, &geometry_attribute);
}
