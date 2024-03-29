// jbwm - Minimalist Window Manager for X
// Copyright 2008-2019, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "title_bar.h"
#include "config.h"
#include "draw.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "font.h"
#include "atom.h"
#include "move_resize.h"
#include "util.h"
#include "wm_state.h"
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#ifdef JBWM_USE_XFT
#include <X11/Xft/Xft.h>
#endif//JBWM_USE_XFT
#include <X11/Xutil.h>
static inline int8_t set_shaded(struct JBWMClient * c) {
  c->old_size.height = c->size.height;
  c->size.height = 1;
  return IconicState;
}
static inline int8_t set_not_shaded(struct JBWMClient * c)
{
  c->size.height = c->old_size.height;
  return NormalState;
}
static inline bool can_shade(struct JBWMClient * c)
{
  // Honor mwm minimize hint.  Abort if fullscreen.
  return !c->opt.no_shade && !c->opt.fullscreen;
}
// This implements window shading, a substitute for iconification.
void jbwm_toggle_shade(struct JBWMClient * c)
{
  if (can_shade(c)){
    Display *d;
    const bool s = c->opt.shaded = !c->opt.shaded;
    const int8_t state = (s ? set_shaded : set_not_shaded)(c);
    d=c->screen->xlib->display;
    (state == IconicState ? jbwm_ewmh_add_state : jbwm_ewmh_remove_state)
    (d, c->window, jbwm_atoms[JBWM_NET_WM_STATE_SHADED]);
    jbwm_move_resize(c);
    jbwm_set_wm_state(c, state);
  }
}
static inline uint16_t mv(Display * const d, Window const w, 
  uint8_t const font_height, uint16_t x) {
  if (w){
    XMoveWindow(d, w, x -= font_height, 0);
  }
  return x;
}
// Return of width allows chain-calling
static uint16_t move_buttons(Display * d,
  struct JBWMClientTitleBar * t,
  const uint16_t width, uint8_t const font_height)
{
  mv(d, t->stick, font_height,
    mv(d, t->shade, font_height,
      mv(d, t->resize, font_height, width)));
  return width;
}
static inline Window get_win(Display * d, const Window p,
  const jbwm_pixel_t bg, uint8_t const font_height)
{
  return XCreateSimpleWindow(d, p, 0, 0, font_height, font_height, 0, 0, bg);
}
static void add_buttons(struct JBWMClient * c,
  const struct JBWMPixels * p, const Window t)
{
  struct JBWMClientOptions * o = &c->opt;
  Display * d = c->screen->xlib->display;
  uint8_t const h = c->screen->font_height;
  c->tb.close = o->no_close ? 0 : get_win(d, t, p->close, h);
  c->tb.resize = o->no_resize ? 0: get_win(d, t, p->resize, h);
  c->tb.shade = o->no_shade ? 0 : get_win(d, t, p->shade, h);
  c->tb.stick = get_win(d, t, p->stick, h);
}
static void configure_title_bar(Display * d, const Window t)
{
  XSelectInput(d, t, ExposureMask);
  XMapRaised(d, t);
  XMapSubwindows(d, t);
  jbwm_grab_button(d, t, None);
}
static Window new_title_bar(struct JBWMClient * c)
{
  struct JBWMScreen *s = c->screen;
  const struct JBWMPixels * p = &s->pixels;
  Display * d = s->xlib->display;
  const Window t = c->tb.win = get_win(d, c->parent, p->bg,
    s->font_height);
  add_buttons(c, p, t);
  configure_title_bar(d, t);
  return t;
}
// Free result with XFree if not NULL
static inline char * jbwm_get_title(Display * d, const Window w)
{
  return jbwm_get_property(d, w, XA_WM_NAME, &(uint16_t){0});
}
static void draw_title(struct JBWMClient * c)
{
  char *name;
  name = jbwm_get_title(c->screen->xlib->display, c->window);
  if (name) {
    jbwm_draw_string(c->screen, c->tb.win, c->screen->font_height+4,
      jbwm_get_font_ascent(), name, strlen(name));
    XFree(name);
  }
}
static void remove_title_bar(struct JBWMClient * c)
{
  ++(c->ignore_unmap);
  XDestroyWindow(c->screen->xlib->display, c->tb.win);
  c->tb.win = 0;
}
static inline void resize_title_bar(Display * d, const Window win,
  struct JBWMClientTitleBar * tb, const uint16_t new_width,
  uint8_t const font_height) {
  // Expand/Contract the title bar width as necessary:
  if (tb)
    XResizeWindow(d, win, move_buttons(d, tb, new_width, font_height),
      font_height);
#ifdef DEBUG
  else
    abort();
#endif//DEBUG
}
void jbwm_update_title_bar(struct JBWMClient * c)
{
  if (!c->opt.shaped){
    Window w;
    w = c->tb.win;
    if (c->opt.fullscreen && w) {
      remove_title_bar(c);
    }else{
      if (!w)
        w = new_title_bar(c);
      {
        Display *d;
        struct JBWMScreen *s = c->screen;
        d = s->xlib->display;
        resize_title_bar(d, w, &c->tb, c->size.width,s->font_height);
        XClearWindow(d, w);
      }
      draw_title(c);
      if (c->opt.no_title_bar)
        remove_title_bar(c);
    }
  }
}
