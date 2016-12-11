// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "snap.h"
#include <stdlib.h>
#include "client.h"
#include "config.h"
#include "font.h"
#include "JBDim.h"
#include "screen.h"
__attribute__ ((const, hot, warn_unused_result))
static int_fast16_t sborder(const int_fast16_t xy, const int_fast16_t
	edge)
{
	if (abs(xy + edge) < JBWM_SNAP)
		  return - edge;
	return xy;
}
void jbwm_snap_border(struct JBWMClient * restrict c)
{
	struct JBWMRectangle * g = &(c->size);
	/* snap to screen border */
	const uint8_t b = 2 * c->border;
	g->x = sborder(g->x, 0 - b);
	const struct JBDim s = jbwm_get_screen(c)->size;
	g->x = sborder(g->x, g->width - s.width + b);
	/* Store the font height statically here in order to reduce
	 * the function call overhead.  */
	static uint8_t fh;
	if (!fh)
		fh = jbwm_get_font_height();
	g->y = sborder(g->y, 0 - (c->opt.no_title_bar ? 0 : fh));
	g->y = sborder(g->y, g->height + b - s.height);
}
__attribute__ ((const, hot, warn_unused_result))
static inline int_fast16_t absmin(const int_fast16_t a, const
	int_fast16_t b)
{
	return abs(a) < abs(b) ? a : b;
}
__attribute__ ((const, hot, warn_unused_result))
static int_fast16_t jbwm_snap_dim(const int_fast16_t cxy, const
	uint_fast16_t cwh, const int_fast16_t cixy, const
	uint_fast16_t ciwh, const uint8_t border, const int_fast16_t d)
{
	const int_fast16_t s = cixy + ciwh - cxy, t = cixy - cxy;
	return  absmin(absmin(absmin(absmin(d, s + border),
		s - cwh), t - cwh - border), t);
}
static struct JBDim search(struct JBWMClient * restrict c)
{
	struct JBDim d = { .x = JBWM_SNAP, .y = JBWM_SNAP };
	for (struct JBWMClient * restrict ci = jbwm_get_head_client();
		ci; ci = ci->next) {
		if ((ci == c) || (ci->screen != c->screen)
		    || (ci->vdesk != c->vdesk))
			continue;
		struct JBWMRectangle * gi = &(ci->size);
		const uint8_t b = c->border;
		if ((gi->y - c->size.height - c->size.y <= d.x)
		    && (c->size.y - gi->height - gi->y <= d.x))
			d.x = jbwm_snap_dim(c->size.x, c->size.width,
				gi->x, gi->width, b, d.x);
		if ((gi->x - c->size.width - c->size.x <= d.y)
		    && (c->size.x - gi->width - gi->x <= d.y))
			d.y = jbwm_snap_dim(c->size.y, c->size.height,
				gi->y, gi->height, b, d.y);
	}
	return d;
}
void jbwm_snap_client(struct JBWMClient * restrict c)
{
	jbwm_snap_border(c);
	// Snap to other windows:
	const struct JBDim d = search(c);
	if (abs(d.x) < JBWM_SNAP)
		c->size.x += d.x;
	if (abs(d.y) < JBWM_SNAP)
		c->size.y += d.y;
}
