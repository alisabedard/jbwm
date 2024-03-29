// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "mwm.h"
#include "ewmh.h"
#include "atom.h"
#include "JBWMClient.h"
#include "log.h"
#include "mwmproto.h"
#include "util.h"
static void process_flags(struct JBWMClientOptions * o)
{
  if (o->tearoff) {
    o->no_resize = o->no_shade = o->no_max
    = o->no_title_bar = true;
    o->border = 0;
  }
}
static void do_functions(struct JBWMClientOptions * o,
  const enum MwmFunctions f)
{
  o->no_close = !(f & MWM_FUNC_CLOSE);
  o->no_max = !(f & MWM_FUNC_MAXIMIZE);
  o->no_move = !(f & MWM_FUNC_MOVE);
  o->no_resize = !(f & MWM_FUNC_RESIZE);
  o->no_shade = !(f & MWM_FUNC_MINIMIZE);
}
static void do_decorations(struct JBWMClientOptions * o,
  const enum MwmDecor f)
{
  o->border = f & MWM_DECOR_BORDER ? 1 : 0;
  o->no_close = !(f & MWM_DECOR_MENU);
  o->no_max = !(f & MWM_DECOR_MAXIMIZE);
  o->no_resize = !(f & MWM_DECOR_RESIZEH);
  o->no_shade = !(f & MWM_DECOR_MINIMIZE);
  o->no_title_bar = !(f & MWM_DECOR_TITLE);
}
void jbwm_handle_mwm_hints(struct JBWMClient * c)
{
  Display * d = c->screen->xlib->display;
  const Atom mwm_hints = jbwm_atoms[JBWM_MOTIF_WM_HINTS];
  struct JBWMMwm * m = jbwm_get_property(d, c->window,
    mwm_hints, &(uint16_t){0});
  if (m) { // property successfully retrieved
    struct JBWMClientOptions * o = &c->opt;
    if (!((c->opt.tearoff = m->flags
          & MWM_HINTS_STATUS && m->status &
          MWM_TEAROFF_WINDOW))) {
      // skip the following if tear-off window
      if (m->flags & MWM_HINTS_FUNCTIONS
        && !(m->functions & MWM_FUNC_ALL))
        do_functions(o, m->functions);
      if (m->flags & MWM_HINTS_DECORATIONS
        && !(m->decor & MWM_DECOR_ALL))
        do_decorations(o, m->decor);
    }
    XFree(m);
    process_flags(o);
  }
}
