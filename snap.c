// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

__attribute__((hot))
static void
sborder(int *xy, const uint8_t border)
{
        if(abs(*xy+border)<JBWM_SNAP)
                *xy=-border;
}
 
void
snap_client_to_screen_border(Client * c)
{
        XSizeHints *g = &(c->size);
        /* snap to screen border */
        const uint8_t b = c->border;
        sborder(&g->x, -b);
        sborder(&g->x, g->width - c->screen->width + 2*b);
        sborder(&g->y, -b-((c->flags&JB_NO_TB)?0:TDIM));
        sborder(&g->y, g->height + 2*b - c->screen->height);
}
 
__attribute__((const,hot))
static int 
absmin(const int a, const int b)
{
        return abs(a)<abs(b)?a:b;
}

static int
snap_dim(const int cxy, const uint16_t cwh, const int cixy, 
        const uint16_t ciwh, const uint8_t border, int d)
{
        int s=cixy+ciwh;
        d = absmin(d, s - cxy+border);
        d = absmin(d, s - cxy - cwh);
        s=cixy-cxy;
        d = absmin(d, s - cwh-border);
        d = absmin(d, s);
        return d;
}
 
void
snap_client(Client * c)
{
        XLOG("snap_client");
        assert(c);
        snap_client_to_screen_border(c);
        // Snap to other windows:
        XSizeHints *g = &(c->size);
        const uint8_t s=JBWM_SNAP;
        Position d = {s, s};
        for(Client *ci = jbwm.head; ci; ci = ci->next)
        {
                if((ci==c)||(ci->screen!=c->screen)||(ci->vdesk!=c->vdesk))
                        continue;
                XSizeHints *gi = &(ci->size);
                const uint8_t b = c->border;
                if((gi->y - g->height - g->y <= s) 
                        && (g->y - gi->height - gi->y <= s))
                {
                        d.x = snap_dim(g->x, g->width, gi->x, gi->width, b, s);
                }
                if((gi->x - g->width - g->x <= s) 
                        && (g->x - gi->width - gi->x <= s))
                {
                        d.y = snap_dim(g->y, g->height, gi->y, gi->height, b, 
                                s);
                }
	}
        if(abs(d.x) < s)
                g->x += d.x;
        if(abs(d.y) < s)
                g->y += d.y;
}
