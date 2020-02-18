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
#include <stdlib.h>
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
__attribute__((const))
static inline int16_t sborderdir(int16_t xy, int16_t const min,
    int16_t const max){
    xy=sborder(xy,min);
    xy=sborder(xy,max);
    return xy;
}
void jbwm_snap_border(struct JBWMClient * restrict c)
{
    struct JBWMScreen *scr = c->screen;
    struct JBWMRectangle * restrict g = &(c->size);
    const struct JBWMSize s = {scr->xlib->width,
        scr->xlib->height};
    const uint8_t b = c->opt.border << 1;
    /*  snap to screen border */
    g->x=sborderdir(g->x,0,g->width-s.width+b);
    g->y=sborderdir(g->y,(c->opt.no_title_bar^1)*-scr->font_height,
        g->height - s.height + b);
}
/* Definition of this as an inline function guarantees no side-effects
 * and minimizes over-expansion (the full expansion of jbwm_snap_dim
 * is approximately a dozen lines).  */
__attribute__ ((const, warn_unused_result))
static inline int16_t absmin(int16_t const a, int16_t const b)
{
    int16_t r;
#if defined(__i386__) || defined(__x86_64__)
    __asm__(
        "movw %%ax, %%bx\n\t" /*  save */
        "negw %%ax\n\t" /*  negate */
        "cmovlw %%bx, %%ax\n\t" /*  restore saved if negative */
        "movw %%cx, %%dx\n\t" /*  save */
        "negw %%cx\n\t" /*  negate */
        "cmovlw %%dx, %%cx\n\t" /*  restore saved if negative */
        "cmpw %%ax, %%cx\n\t" /*  abs(b)<abs(a) (note reversed at&t syntax) */
        "cmovlw %%dx, %%bx\n\t" /*  b (orig a) to output if < */
        /*       "int3\n\t" // debug */
        : "=b" (r)
        : "a" (a), "c" (b)
        : "%dx"
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
static struct JBWMPoint snap_search(struct JBWMClient * c)
{
    struct JBWMPoint d = {JBWM_SNAP, JBWM_SNAP};
    struct JBWMRectangle const s = c->size;
    for (struct JBWMClient * ci = *(c->head);
        ci; ci = ci->next) {
        if ((ci != c) && (ci->screen == c->screen)
            && (ci->vdesk == c->vdesk)) {
            struct JBWMRectangle const gi = ci->size;
            if ((gi.y - s.height - s.y <= d.x) &&
                (s.y - gi.height - gi.y <= d.x))
                d.x = jbwm_snap_dim(s.x, s.width,
                    gi.x, gi.width, d.x);
            if ((gi.x - s.width - s.x <= d.y) &&
                (s.x - gi.width - gi.x <= d.y))
                d.y = jbwm_snap_dim(s.y, s.height,
                    gi.y, gi.height, d.y);
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
