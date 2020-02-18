/*  jbwm - Minimalist Window Manager for X */
/*  Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com> */
/*  Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk> */
/*  See README for license and other details. */
#include "snap.h"
/***/
#include "config.h"
#include "JBWMClient.h"
#include "JBWMPoint.h"
#include "JBWMSize.h"
/***/
#ifdef JBWM_DEBUG_ABSMIN
#include <stdio.h>
#endif/* JBWM_DEBUG_ABSMIN */
__attribute__ ((const, warn_unused_result))
static inline int16_t sborder(const int16_t xy, const int16_t edge)
{
    int32_t r;
#if defined(__i386__) || defined(__x86_64__)
    __asm__(
        "movw %%bx, %%ax\n\t" /*  copy xy */
        "addw %%cx, %%ax\n\t" /* xy + edge, preserve cx for later */
        "movw %%ax, %%dx\n\t" /*  copy result */
        "negw %%ax\n\t" /*  negate copy */
        "cmovlw %%dx, %%ax\n\t" /*  if negative copy, restore src - abs */
        "negw %%cx\n\t" /*  -edge */
        "cmpw %1, %%ax\n\t" /*  note reversed at&t operation order */
        "cmovgew %%bx,%%cx\n\t" /*  result is xy if >= */
        : "=c" (r)
        : "i" (JBWM_SNAP), "b" (xy), "c" (edge)
        : "%ax", "%dx" /*  scratch */
    );
#else/*  Portable version: */
    r = abs(xy + edge) < JBWM_SNAP ? -edge : xy;
#endif
    return r;
}
void jbwm_snap_border(struct JBWMClient * restrict c)
{
    int16_t x,y;
    struct JBWMScreen *scr = c->screen;
    struct JBWMRectangle * restrict g = &(c->size);
    const struct JBWMSize s = {scr->xlib->width,
        scr->xlib->height};
    const uint8_t b = c->opt.border << 1;
    /*  snap to screen border */
    x=g->x;
    x = sborder(x, 0);
    x = sborder(x, g->width - s.width + b);
    g->x=x;
    y=g->y;
    y = sborder(y, (c->opt.no_title_bar^1)*-scr->font_height);
    y = sborder(y, g->height - s.height + b);
    g->y=y;
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
        "movl %%eax, %%ebx\n\t" /*  save */
        "negl %%eax\n\t" /*  negate */
        "cmovll %%ebx, %%eax\n\t" /*  restore saved if negative */
        "movl %%ecx, %%edx\n\t" /*  save */
        "negl %%ecx\n\t" /*  negate */
        "cmovll %%edx, %%ecx\n\t" /*  restore saved if negative */
        "cmpl %%eax, %%ecx\n\t" /*  abs(b)<abs(a) (note reversed at&t syntax) */
        "cmovll %%edx, %%ebx\n\t" /*  b (orig a) to output if < */
        /*       "int3\n\t" // debug */
        : "=b" (r)
        : "a" (a), "c" (b)
        : "%edx"
    );
#else/*  Portable version: */
    r = abs(a) < abs(b) ? a : b;
#endif
#ifdef JBWM_DEBUG_ABSMIN
    printf("%d\t",r);
#endif/* JBWM_DEBUG_ABSMIN */
    return r;
}
__attribute__ ((const, warn_unused_result))
static inline int jbwm_snap_dim(const int cxy, const int cwh, const int cixy,
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
static inline struct JBWMPoint snap_search(struct JBWMClient * c)
{
    struct JBWMPoint d = {JBWM_SNAP, JBWM_SNAP};
    for (struct JBWMClient * ci = *(c->head);
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
    /*  Snap to other windows: */
    const struct JBWMPoint d = snap_search(c);
#if defined(__i386__) || defined(__x86_64__)
    __asm__(
        "movl %%ebx, %%eax\n\t"
        "negl %%eax\n\t"
        "cmovll %%ebx, %%eax\n\t" /*  abs(ebx) to eax */
        "movl %%ecx,%%edx\n\t"
        "addl %%ebx,%%edx\n\t"
        "cmpl %1, %%eax\n\t"
        "cmovll  %%edx, %%ecx\n\t"
        : "=c" (c->size.x)
        : "i" (JBWM_SNAP), "b" (d.x), "c" (c->size.x)
        : "%eax","%edx"
    );
    __asm__(
        "movl %%ebx, %%eax\n\t"
        "negl %%eax\n\t"
        "cmovll %%ebx, %%eax\n\t" /*  abs(ebx) to eax */
        "movl %%ecx,%%edx\n\t"
        "addl %%ebx,%%edx\n\t"
        "cmpl %1, %%eax\n\t"
        "cmovll  %%edx, %%ecx\n\t"
        : "=c" (c->size.y)
        : "i" (JBWM_SNAP), "b" (d.y), "c" (c->size.y)
        : "%eax","%edx"
    );

#else /*  portable */
   if (abs(d.x) < JBWM_SNAP)
        c->size.x += d.x;
   if (abs(d.y) < JBWM_SNAP)
        c->size.y += d.y;
#endif
}
