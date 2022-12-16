// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "button_event.h"
#include "client.h"
#include "drag.h"
#include "log.h"
#include "title_bar.h"
#include "wm_state.h"
static void handle_title_bar_button(XButtonEvent * e, struct JBWMClient * c,
  struct JBWMClient ** head_client, struct JBWMClient ** current_client)
{
  JBWM_LOG("e->window: %d, c->title_bar: %d, e->subwindow: %d",
    (int)e->window, (int)c->tb.win, (int)e->subwindow);
  JBWM_LOG("c->tb.close: %d", (int)c->tb.close);
  JBWM_LOG("e->button: %d", (int)e->button);
  struct JBWMClientOptions * o = &c->opt;
  if (!e->subwindow)
    jbwm_drag(c, head_client, false);
  else if (e->subwindow == c->tb.close && !o->no_close)
    jbwm_send_wm_delete(c);
  else if (e->subwindow == c->tb.resize && !o->no_resize)
    jbwm_drag(c, head_client, !c->opt.no_resize);
  else if (e->subwindow == c->tb.shade && !o->no_shade)
    jbwm_toggle_shade(c);
  else if (e->subwindow == c->tb.stick)
    jbwm_toggle_sticky(c, current_client);
  else
    jbwm_drag(c, head_client, false);
}
void jbwm_handle_button_event(XButtonEvent * e, struct JBWMClient * c,
  struct JBWMClient ** head_client,
  struct JBWMClient ** current_client)
{
  JBWM_LOG("jbwm_handle_button_event");
  if (c) { // keep this check, added in response to segfault
    const bool fs = c->opt.fullscreen;
    Display * d = e->display;
    switch (e->button) {
    case Button1:
      if (fs)
        XRaiseWindow(d, c->parent);
      else
        handle_title_bar_button(e, c, head_client, current_client);
      break;
    case Button2:
      XLowerWindow(d, c->parent);
      break;
    case Button3:
      /* Resize operations more useful here,
         rather than for third button, for laptop
         users especially, where it is difficult
         to register a middle button press, even
         with X Emulate3Buttons enabled.  */
      if (fs)
        XLowerWindow(d, c->parent);
      else
        jbwm_drag(c, head_client, !c->opt.shaded);
      break;
    }
  }
}
