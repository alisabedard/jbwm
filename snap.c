// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "snap.h"
#include <stdlib.h>
#include "client.h"
#include "config.h"
#include "font.h"
#include "geometry.h"
#include "JBWMClient.h"
#include "JBWMPoint.h"
#include "JBWMSize.h"
#include "log.h"
#include <stdio.h>
__attribute__ ((const, warn_unused_result))
static inline int sborder(const int xy, const int edge)
{
    int r;
#if defined(__i386__) || defined(__x86_64__)
    __asm__(
        "movl %%ebx, %%eax\n\t" // copy xy
        "addl %%ecx, %%eax\n\t" //xy + edge, preserve ecx for later
        "movl %%eax, %%edx\n\t" // copy result
        "negl %%eax\n\t" // negate copy
        "cmovll %%edx, %%eax\n\t" // if negative copy, restore src - abs
        "neg %%ecx\n\t" // -edge
        "cmpl %1, %%eax\n\t" // note reversed at&t operation order
        "cmovgel %%ebx,%%ecx\n\t" // result is xy if >=
        : "=c" (r)
        : "i" (JBWM_SNAP), "b" (xy), "c" (edge)
        : "%eax", "%edx" // scratch
    );
#else// Portable version:
    r = abs(xy + edge) < JBWM_SNAP ? -edge : xy;
#endif
    return r;
}
void jbwm_snap_border(struct JBWMClient * restrict c)
{
    struct JBWMScreen *scr = c->screen;
    struct JBWMRectangle * restrict g = &(c->size);
    const struct JBWMSize s = {scr->xlib->width,
        scr->xlib->height};
    const uint8_t b = c->opt.border << 1;
    // snap to screen border
    g->x = sborder(g->x, 0);
    g->x = sborder(g->x, g->width - s.width + b);
    g->y = sborder(g->y, c->opt.no_title_bar ? 0 :-scr->font_height);
    g->y = sborder(g->y, g->height - s.height + b);
}
/* Definition of this as an inline function guarantees no side-effects
 * and minimizes over-expansion (the full expansion of jbwm_snap_dim
 * is approximately a dozen lines).  */
__attribute__ ((const, warn_unused_result))
static inline int absmin(int const a, int const b)
{
    int r;
#if defined(__i386__) || defined(__x86_64__)
    __asm__(
        "movl %%eax, %%ebx\n\t" // save
        "negl %%eax\n\t" // negate
        "cmovll %%ebx, %%eax\n\t" // restore saved if negative
        "movl %%ecx, %%edx\n\t" // save
        "negl %%ecx\n\t" // negate
        "cmovll %%edx, %%ecx\n\t" // restore saved if negative
        "cmpl %%eax, %%ecx\n\t" // abs(b)<abs(a) (note reversed at&t syntax)
        "cmovll %%edx, %%ebx\n\t" // b (orig a) to output if <
//      "int3\n\t" // debug
        : "=b" (r)
        : "a" (a), "c" (b)
        : "%edx"
    );
#else// Portable version:
    r = abs(a) < abs(b) ? a : b;
#endif
#ifdef JBWM_DEBUG_ABSMIN
    printf("%d\t",r);
#endif//JBWM_DEBUG_ABSMIN
    return r;
}
__attribute__ ((const, warn_unused_result))
static int jbwm_snap_dim(const int cxy, const int cwh, const int cixy,
    const int ciwh, const int d)
{
    const int s = cixy + ciwh - cxy, t = cixy - cxy;
    return absmin(absmin(absmin(absmin(d, s), s - cwh), t - cwh), t);
}
/* Don't use restrict for struct JBWMClient withing this function, as
 * c and ci may alias each other.  Qualifier restrict is fine for struct
 * JBWMRectangle.  This is performance critical, scaling O(n)
 * relative to the number of windows, so leave iterative in definition to
 * avoid further overhead.  */
static struct JBWMPoint snap_search(struct JBWMClient * c)
{
    struct JBWMPoint d = {JBWM_SNAP, JBWM_SNAP};
    for (struct JBWMClient * ci = jbwm_get_head_client();
        ci; ci = ci->next) {
        if ((ci != c) && (ci->screen == c->screen)
            && (ci->vdesk == c->vdesk)) {
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
    }
    return d;
}
void jbwm_snap_client(struct JBWMClient * restrict c)
{
    jbwm_snap_border(c);
    // Snap to other windows:
    const struct JBWMPoint d = snap_search(c);
    if (abs(d.x) < JBWM_SNAP)
        c->size.x += d.x;
    if (abs(d.y) < JBWM_SNAP)
        c->size.y += d.y;
}
