// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "events.h"
#include <X11/Xatom.h>
#include "button_event.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_client.h"
#include "ewmh_client_list.h"
#include "JBWMScreen.h"
#include "key_event.h"
#include "log.h"
#include "move_resize.h"
#include "new.h"
#include "select.h"
#include "title_bar.h"
#include "util.h"
#include "wm_state.h"
// Set log level for events
#define JBWM_LOG_EVENTS 1
static struct JBWMScreen * get_screen(struct JBWMScreen *s,
  const Window root, const int i) {
  return RootWindowOfScreen(s[i].xlib) == root ? s
  // Check that the next iteration is not outside array bounds, using
  // suppporting field as validation
  : s[i + 1].supporting ? get_screen(s,root, i + 1) : s;
}
static void jbwm_handle_PropertyNotify(XEvent * ev, struct JBWMClient * c)
{
  if (c) {
    XPropertyEvent * e = &ev->xproperty;
    if (e->state != PropertyNewValue)
      return;
    if (e->atom == XA_WM_NAME)
      jbwm_update_title_bar(c);
    else {
      Display * d = e->display;
#if JBWM_LOG_EVENTS > 3
      jbwm_print_atom(d, e->atom, __FILE__, __LINE__);
#endif//JBWM_LOG_EVENTS > 3
      if (e->atom == jbwm_get_wm_state(d))
        jbwm_move_resize(c);
    }
  }
}
static void jbwm_handle_MapRequest(XEvent * ev, struct JBWMClient * c,
  struct JBWMScreen * s, struct JBWMClient ** head_client,
  struct JBWMClient ** current_client) {
  if (!c) {
    XMapRequestEvent * e = &ev->xmaprequest;
    /* This check fixes a race condition in old libreoffice and
     * certain Motif dialogs where an attempt is made to
     * request mapping twice: */
    static unsigned long serial;
    if (e->serial != serial){
      serial = e->serial;
      JBWM_LOG("jbwm_handle_MapRequest(), send_event:%d",
        e->send_event);
      jbwm_new_client(get_screen(s, e->parent, 0), head_client,
        current_client, e->window);
    }
  }
}
static void jbwm_handle_ColormapNotify(XEvent * ev, struct JBWMClient * c)
{
  XColormapEvent * e = &ev->xcolormap;
  if (c && e->new)
    XInstallColormap(e->display, c->cmap = e->colormap);
}
static void jbwm_handle_ConfigureNotify(XEvent * ev, struct JBWMClient * c)
{
  (void)ev;
  if (c && !ev->xconfigure.override_redirect)
    jbwm_move_resize(c);
}
static void jbwm_handle_ConfigureRequest(XEvent * ev, struct JBWMClient * c)
{
  XConfigureRequestEvent * e = &ev->xconfigurerequest;
  JBWM_LOG("handle_configure_request():"
    "x: %d, y: %d, w: %d, h: %d, b: %d",
    e->x, e->y, e->width, e->height, e->border_width);
  XConfigureWindow(e->display, e->window, e->value_mask,
    &(XWindowChanges){ .x = e->x, .y = e->y,
      .width = e->width, .height = e->height,
      .border_width = e->border_width,
      .sibling = e->above, .stack_mode = e->detail});
  XSync(e->display, false);
  if (c)
    jbwm_move_resize(c);
}
static void jbwm_handle_EnterNotify(XEvent * ev, struct JBWMClient * c,
  struct JBWMClient ** current_client)
{
  if (c && ev->xcrossing.window == c->parent)
    jbwm_select_client(c, current_client);
}
static void jbwm_handle_Expose(XEvent * ev, struct JBWMClient * c)
{
  if (c && !ev->xexpose.count)
    jbwm_update_title_bar(c);
}
void jbwm_events_loop(struct JBWMScreen * s, struct JBWMClient ** head_client,
  struct JBWMClient ** current_client)
{
  Display *d;
  d=s->xlib->display;
  for (;;) {
    XEvent ev;
    XNextEvent(d, &ev);
    struct JBWMClient * c = jbwm_find_client(*head_client, ev.xany.window);
    if (c)
      s = c->screen;
    switch (ev.type) {
    case ConfigureNotify:
      jbwm_handle_ConfigureNotify(&ev,c);
      break;
    case ConfigureRequest:
      jbwm_handle_ConfigureRequest(&ev,c);
      break;
    case KeyPress:
      JBWM_LOG("c: %d, window: %d, root: %d", (int)c,
        (int)ev.xkey.window, (int)ev.xkey.root);
      if (!c)
        c = *current_client;
      jbwm_handle_key_event(s, c, head_client, current_client, &ev.xkey);
      break;
    case ButtonPress:
      jbwm_handle_button_event(&ev.xbutton, c, head_client,
        current_client);
      break;
    case EnterNotify:
      jbwm_handle_EnterNotify(&ev, c, current_client);
      break;
    case Expose:
      jbwm_handle_Expose(&ev,c);
      break;
    case CreateNotify:
    case DestroyNotify:
      jbwm_ewmh_update_client_list(*head_client);
      break;
    case UnmapNotify:
      if (c && (c->opt.remove || (c->ignore_unmap--<1)))
        jbwm_client_free(c, head_client, current_client);
      break;
    case MapRequest:
      jbwm_handle_MapRequest(&ev, c, s, head_client, current_client);
      break;
    case PropertyNotify:
      jbwm_handle_PropertyNotify(&ev,c);
      break;
    case ColormapNotify:
      jbwm_handle_ColormapNotify(&ev,c);
      break;
    case ClientMessage:
      jbwm_ewmh_handle_client_message(&ev.xclient, c, head_client,
        current_client);
      break;
      // ignore
    case ButtonRelease:
    case KeyRelease:
    case MapNotify:
    case MappingNotify:
    case MotionNotify:
    case ReparentNotify:
    default:
      break;
    }
  }
}

