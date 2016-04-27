// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <stdint.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include "client_t.h"
#include "config.h"
#include "jbwmenv.h"
#include "log.h"
#include "titlebar.h"

__attribute__ ((const, hot, warn_unused_result, regparm(2)))
static int16_t sborder(const int16_t xy, const int16_t edge)
{
	if(abs(xy+edge) < JBWM_SNAP)
		  return - edge;
	return xy;
}

void snap_border(Client *restrict c)
{
	XSizeHints *restrict g = &(c->size);
	/* snap to screen border */
	const uint8_t b = 2 * c->border;
	g->x=sborder(g->x, 0 - b);
	g->x=sborder(g->x, g->width - c->screen->size.w + b);
	g->y=sborder(g->y, 0 - (c->flags & JB_NO_TB ? 0 : TDIM));
	g->y=sborder(g->y, g->height + b - c->screen->size.h);
}

__attribute__ ((const, hot, warn_unused_result, regparm(2)))
static inline int16_t absmin(const int16_t a, const int16_t b)
{
	return abs(a) < abs(b) ? a : b;
}

__attribute__ ((const))
static int16_t
snap_dim(const int16_t cxy, const uint16_t cwh, const int16_t cixy,
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

void snap_client(Client * c)
{
	LOG("snap_client");
	snap_border(c);
	// Snap to other windows:
	XSizeHints *restrict g = &(c->size);
	XPoint d = { JBWM_SNAP, JBWM_SNAP };

	for (Client * ci = jbwm.head; ci; ci = ci->next) {
		// This test qualifies 'restrict'
		if ((ci == c) || (ci->screen != c->screen)
		    || (ci->vdesk != c->vdesk))
			continue;

		XSizeHints *restrict gi = &(ci->size);
		const uint8_t b = c->border;

		if ((gi->y - g->height - g->y <= d.x)
		    && (g->y - gi->height - gi->y <= d.x)) {
			d.x = snap_dim(g->x, g->width, gi->x, gi->width, b,
				       d.x);
		}

		if ((gi->x - g->width - g->x <= d.y)
		    && (g->x - gi->width - gi->x <= d.y)) {
			d.y = snap_dim(g->y, g->height, gi->y, gi->height, b,
				       d.y);
		}
	}

	if (abs(d.x) < JBWM_SNAP)
		g->x += d.x;

	if (abs(d.y) < JBWM_SNAP)
		g->y += d.y;
}
