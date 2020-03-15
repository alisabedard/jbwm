/*  jbwm - Minimalist Window Manager for X */
/*  Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com> */
/*  Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk> */
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
void jbwm_snap_border(struct JBWMClient * c)
{
  struct JBWMScreen *scr = c->screen;
  union JBWMRectangle * g = &(c->size);
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
__attribute__((const, warn_unused_result))
static inline bool snap_cond(int16_t const xy, int16_t const wh,
  int16_t const ixy, int16_t const iwh){
#if defined(__x86_64__) || defined(__i386__)
  bool r;
  __asm__(
    "movw %%ax, %%di\n\t" // save ixy in %di
    "subw %%bx, %%ax\n\t" // ixy-wh
    "subw %%cx, %%ax\n\t" // -xy
    "xorw %%bx, %%bx\n\t" // clear (false)
    "movw $1, %%si\n\t" // use %si as a 1 (true) register
    "cmpw %1, %%ax\n\t" // compare against JBWM_SNAP
    "cmovlew %%si, %%bx\n\t" // %bx true if %cx<=JBWM_SNAP
    "subw %%dx, %%cx\n\t" // xy-iwh
    "subw %%di, %%cx\n\t" // -ixy
    "xorw %%ax, %%ax\n\t" // clear (false)
    "cmpw %1, %%cx\n\t" // compare against JBWM_SNAP
    "cmovlew %%si, %%ax\n\t" // %ax true if %cx<=JBWM_SNAP
    "andb %%bl, %%al\n\t" // %a & %b in %a
    :"=a"(r)
    :"i"(JBWM_SNAP),"a"(ixy),"b"(wh),"c"(xy),"d"(iwh)
    :"%di","%si"
  );
  return r;
#else// Portable:
  return (ixy - wh - xy <= JBWM_SNAP) && (xy-iwh-ixy<=JBWM_SNAP);
#endif
}
static inline void adjust_for_titlebar(union JBWMRectangle * geo,
  uint8_t const font_height, bool const titled){
  if(titled){
    geo->y-=font_height;
    geo->height+=font_height;
  }
}
/* Don't use for struct JBWMClient withing this function, as
 * c and ci may alias each other.  Qualifier is fine for struct
 * JBWMRectangle.  This is performance critical, scaling O(n)
 * relative to the number of windows, so leave iterative in definition to
 * avoid further overhead.  */
static union JBWMPoint snap_search(struct JBWMClient * c,
  struct JBWMClient ** head_client)
{
  union JBWMPoint d;
  union JBWMRectangle s = c->size;
  struct JBWMScreen *scr=c->screen;
  bool const ctb = !c->opt.no_title_bar;
  uint8_t const fh = scr->font_height;
  adjust_for_titlebar(&s,fh,ctb);
  d.x=d.y=JBWM_SNAP;
  for (struct JBWMClient * ci = *head_client;
    ci; ci = ci->next) {
    if ((ci != c) && (ci->screen == scr)
      && (ci->vdesk == c->vdesk)) {
      union JBWMRectangle gi = ci->size;
      bool const citb = !ci->opt.no_title_bar;
      adjust_for_titlebar(&gi,fh,citb);
      if(snap_cond(s.y, s.height, gi.y, gi.height))
        d.x = jbwm_snap_dim(s.x, s.width,
          gi.x, gi.width, d.x);
      if(snap_cond(s.x, s.width, gi.x, gi.width))
        d.y = jbwm_snap_dim(s.y, s.height,
          gi.y, gi.height, d.y);
    }
  }
  return d;
}
static inline void snap_dir(int16_t *out, int16_t diff){
#if defined(__i386__) || defined(__x86_64__)
  __asm__ (
    "movw %%bx, %%ax\n\t"
    "negw %%ax\n\t"
    "cmovlw %%bx, %%ax\n\t" /*  abs(bx) to ax */
    "movw %%cx,%%dx\n\t"
    "addw %%bx,%%dx\n\t"
    "cmpw %1, %%ax\n\t"
    "cmovlw  %%dx, %%cx\n\t"
    : "=c" (*out)
    : "i" (JBWM_SNAP), "b" (diff), "c" (*out)
    : "%ax","%dx"
  );
#else /* portable */
  if(abs(diff)<JBWM_SNAP)
    *out+=diff;
#endif
}
void jbwm_snap_client(struct JBWMClient * c, struct JBWMClient ** head_client)
{
  jbwm_snap_border(c);
  /*  Snap to other windows: */
  const union JBWMPoint d = snap_search(c, head_client);
  snap_dir(&c->size.array[0],d.array[0]);
  snap_dir(&c->size.array[1],d.array[1]);
}
