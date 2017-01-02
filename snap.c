// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "snap.h"
#include <stdlib.h>
#include "JBWMSize.h"
#include "JBWMClient.h"
#include "JBWMPoint.h"
#include "JBWMScreen.h"
#include "client.h"
#include "config.h"
#include "font.h"
#include "screen.h"
__attribute__ ((const, warn_unused_result))
static int sborder(const int xy, const int edge)
{
	if (abs(xy + edge) < JBWM_SNAP)
		return - edge;
	return xy;
}
void jbwm_snap_border(struct JBWMClient * restrict c)
{
	struct JBWMRectangle * restrict g = &(c->size);
	// snap to screen border
	g->x = sborder(g->x, 0);
	const struct JBWMSize s = jbwm_get_screen(c)->size;
	const uint8_t b = c->border * 2;
	g->x = sborder(g->x, g->width - s.width + b);
	g->y = sborder(g->y, c->opt.no_title_bar ? 0 :
		- jbwm_get_font_height());
	g->y = sborder(g->y, g->height - s.height + b);
}
/* Definition of this as an inline function guarantees no side-effects
 * and minimizes over-expansion (the full expansion of jbwm_snap_dim
 * is approximately a dozen lines).  */
__attribute__ ((const, warn_unused_result))
static inline int absmin(const int a, const int b)
{
	return abs(a) < abs(b) ? a : b;
}
__attribute__ ((const, warn_unused_result))
static int jbwm_snap_dim(const int cxy, const int cwh, const int cixy,
	const int ciwh, const int d)
{
	const int s = cixy + ciwh - cxy, t = cixy - cxy;
	return absmin(absmin(absmin(absmin(d, s), s - cwh), t - cwh), t);
}
/* Don't use restrict for struct JBWMClient withing this function, as
 * c and ci may alias each other.  It is fine for struct
 * JBWMRectangle.  */
static struct JBWMPoint search(struct JBWMClient * c)
{
	struct JBWMPoint d = {JBWM_SNAP, JBWM_SNAP};
	for (struct JBWMClient * ci = jbwm_get_head_client();
		ci; ci = ci->next) {
		if ((ci == c) || (ci->screen != c->screen)
			|| (ci->vdesk != c->vdesk))
			continue;
		struct JBWMRectangle * restrict gi = &(ci->size);
		if ((gi->y - c->size.height - c->size.y <= d.x) &&
			(c->size.y - gi->height - gi->y <= d.x))
			d.x = jbwm_snap_dim(c->size.x, c->size.width,
				gi->x, gi->width, d.x);
		if ((gi->x - c->size.width - c->size.x <= d.y) &&
			(c->size.x - gi->width - gi->x <= d.y))
			d.y = jbwm_snap_dim(c->size.y, c->size.height,
				gi->y, gi->height, d.y);
	}
	return d;
}
void jbwm_snap_client(struct JBWMClient * restrict c)
{
	jbwm_snap_border(c);
	// Snap to other windows:
	const struct JBWMPoint d = search(c);
	if (abs(d.x) < JBWM_SNAP)
		c->size.x += d.x;
	if (abs(d.y) < JBWM_SNAP)
		c->size.y += d.y;
}
