// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "vdesk.h"
#include "client.h"
#include "config.h"
#include "draw.h"
#include "ewmh.h"
#include "font.h"
#include "log.h"
#include "atom.h"
#include "JBWMClient.h"
#include "JBWMScreen.h"
#include "util.h"
#include <X11/Xatom.h> // keep
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#ifdef JBWM_USE_XOSD
#include <xosd.h>
#endif//JBWM_USE_XOSD
static void check_visibility(struct JBWMClient * c,
  const uint8_t v)
{
  if (c) {
    if (c->vdesk == v || c->opt.sticky) {
      // allow moving windows by sticking
      c->vdesk = v;
      jbwm_restore_client(c);
    } else
      jbwm_hide_client(c);
    check_visibility(c->next, v);
  }
}
#ifndef JBWM_USE_XOSD
static void *delayed_clear_thread(void *data) {
  Display *d;
  struct JBWMScreen * s = (struct JBWMScreen *)data;
  d=s->xlib->display;
  JBWM_LOG("pausing");
  sleep(1);
  XClearWindow(d, s->xlib->root);
  XFlush(d); // must run before thread terminates
  JBWM_LOG("window cleared");
  return NULL;
}
static void delayed_clear(struct JBWMScreen *s){
  pthread_t thread;
  int r;
  r = pthread_create(&thread, NULL, delayed_clear_thread, (void*)s);
  assert(!r);
  JBWM_LOG("thread launched");
}
#endif//!JBWM_USE_XOSD
#ifdef JBWM_USE_XOSD
static xosd *osd;
static void clean_osd(void){
  JBWM_LOG("xosd_destroy");
  xosd_destroy(osd);
}
#endif//JBWM_USE_XOSD
static void show_desktop(struct JBWMScreen *s){
#ifdef JBWM_USE_XOSD
  char buf[4]; // accomodate up to "255", plus '-', plus NUL
  if (!osd){
    osd = xosd_create(1);
    xosd_set_timeout(osd, 1);
    xosd_set_colour(osd, "white");
    xosd_set_font(osd, "variable");
    xosd_set_shadow_offset(osd, 2);
    xosd_set_align(osd, XOSD_center);
    xosd_set_pos(osd, XOSD_middle);
    xosd_set_outline_colour(osd, "black");
    xosd_set_outline_offset(osd, 1);
    // set up cleanup routine at exit
    atexit(clean_osd);
  }
  snprintf(buf, sizeof(buf), "%d", s->vdesk);
  xosd_display(osd, 0, XOSD_string, buf);
#else//!JBWM_USE_XOSD
  enum { BORDER = 4 };
  Display *d;
  char buf[4]; // accomodate up to "255", plus '-', plus NUL
  uint8_t nul_index;
  Window const r = s->xlib->root;
  d=s->xlib->display;
  XClearWindow(d, r);
  nul_index = snprintf(buf, sizeof(buf), "%d", s->vdesk);
  jbwm_draw_string(s, r, BORDER, BORDER+jbwm_get_font_ascent(),
    buf, nul_index);
  delayed_clear(s);
#endif//JBWM_USE_XOSD
}
uint8_t jbwm_set_vdesk(struct JBWMScreen *s,
  struct JBWMClient *head, uint8_t v)
{
  Display *d;
  check_visibility(head, v);
  s->vdesk = v;
  d=s->xlib->display;
  // The data (v) must be a 32 bit type.
  XChangeProperty(d, s->xlib->root, jbwm_atoms[JBWM_NET_CURRENT_DESKTOP],
    XA_CARDINAL, 8, PropModeReplace, (unsigned char *)&v, 1);
  show_desktop(s);
  return v;
}
