// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "snap.h"
#include "client.h"
#include "config.h"
#include "font.h"
#include "jbwm.h"
#include <stdlib.h>
__attribute__ ((const, hot, warn_unused_result))
static int16_t sborder(const int16_t xy, const int16_t edge)
{
	if(abs(xy+edge) < JBWM_SNAP)
		  return - edge;
	return xy;
}
void jbwm_snap_border(struct JBWMClient * restrict c)
{
	struct JBWMRectangle * restrict g = &(c->size);
	/* snap to screen border */
	const uint8_t b = 2 * c->border;
	g->x = sborder(g->x, 0 - b);
	const struct JBDim s = jbwm_get_screens()[c->screen].size;
	g->x = sborder(g->x, g->width - s.width + b);
	g->y = sborder(g->y, 0 - (c->opt.no_title_bar
		? 0 : jbwm_get_font_height()));
	g->y = sborder(g->y, g->height + b - s.height);
}
__attribute__ ((const, hot, warn_unused_result))
static inline int16_t absmin(const int16_t a, const int16_t b)
{
	return abs(a) < abs(b) ? a : b;
}
__attribute__ ((const))
static int16_t
jbwm_snap_dim(const int16_t cxy, const uint16_t cwh, const int16_t cixy,
	 const uint16_t ciwh, const uint8_t border, int16_t d)
{
	int s = cixy + ciwh;
	d = absmin(d, s - cxy + border);
	d = absmin(d, s - cxy - cwh);
	s = cixy - cxy;
	d = absmin(d, s - cwh - border);
	d = absmin(d, s);
	return d;
}
static XPoint search(struct JBWMClient * c)
{
	XPoint d = { JBWM_SNAP, JBWM_SNAP };
	for (struct JBWMClient * ci = jbwm_get_head_client();
		ci; ci = ci->next) {
		// This test qualifies 'restrict'
		if ((ci == c) || (ci->screen != c->screen)
		    || (ci->vdesk != c->vdesk))
			continue;
		struct JBWMRectangle * restrict gi = &(ci->size);
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
void jbwm_snap_client(struct JBWMClient * c)
{
	jbwm_snap_border(c);
	// Snap to other windows:
	const XPoint d = search(c);
	if (abs(d.x) < JBWM_SNAP)
		c->size.x += d.x;
	if (abs(d.y) < JBWM_SNAP)
		c->size.y += d.y;
}
