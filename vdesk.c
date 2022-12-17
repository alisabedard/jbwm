// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "vdesk.h"
#include "client.h"
#include "config.h"
#include "ewmh.h"
#include "font.h"
#include "atom.h"
#include "JBWMClient.h"
#include "JBWMScreen.h"
#include "util.h"
#include <X11/Xatom.h> // keep
#include <stdio.h>
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
static void show_desktop(struct JBWMScreen *s){
  enum { BORDER = 2 };
  Display *d;
  char buf[4]; // accomodate up to "255", plus '-', plus NUL
  uint8_t const desk = s->vdesk;
  uint8_t nul_index;
  Window const r = s->xlib->root;
  d=s->xlib->display;
  XClearWindow(d, r);
  nul_index= snprintf(buf, sizeof(buf), "%d", s->vdesk);
#ifdef JBWM_USE_XFT
  XftDraw *xd;
  xd = s->xft;
  XftDrawChange(xd, r);
  XftDrawStringUtf8(xd, &s->font_color, jbwm_get_font(),
    BORDER, jbwm_get_font_ascent()+BORDER, (XftChar8 *)buf, nul_index);
#else//!JBWM_USE_XFT
  XDrawString(d,r,s->gc,BORDER,jbwm_get_font_ascent()+BORDER, buf, nul_index);
#endif//JBWM_USE_XFT
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
