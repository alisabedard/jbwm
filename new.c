// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "new.h"
#include "client.h"
#include "config.h" // for JBWM_MAX_DESKTOPS
#include "ewmh.h"
#include "geometry.h"
#include "atom.h"
#include "JBWMClient.h"
//#include "keys.h"
#include "log.h" // for JBWM_LOG
#include "mwm.h" // for jbwm_handle_mwm_hints
#include "select.h"
#include "shape.h" // for jbwm_new_shaped_client
#include "snap.h"
#include <stdlib.h>
#include "util.h"
#include "vdesk.h"
#include <X11/Xatom.h> // for XA_CARDINAL
static uint8_t wm_desktop(Display * d, const Window w, uint8_t vdesk)
{
  uint16_t n;
  const Atom a = jbwm_atoms[JBWM_NET_WM_DESKTOP];
  unsigned long *lprop = jbwm_get_property(d, w, a, &n);
  if (lprop) { // See if client has a preferred desktop (as on restart)
    if (n) // is valid
      vdesk = (uint8_t)lprop[0]; // Set vdesk to property value
    XFree(lprop);
  }
  // Set the property in any case to ensure it exists henceforth.
  XChangeProperty(d, w, a, XA_CARDINAL, 8, PropModeReplace,
    (unsigned char *)&vdesk, 1);
  JBWM_LOG("wm_desktop(w: %d): vdesk is %d\n", (int) w, vdesk);
  return vdesk;
}
static inline uint8_t get_vdesk(struct JBWMClient *  c)
{
  return wm_desktop(c->screen->xlib->display, c->window, c->screen->vdesk);
}
__attribute__((nonnull))
static Window get_parent(struct JBWMClient *  c)
{
  enum {
    CFP = CopyFromParent,
    CW_VM = CWOverrideRedirect | CWEventMask,
    WA_EM = SubstructureRedirectMask | SubstructureNotifyMask |
    ButtonPressMask | EnterWindowMask
  };
  union JBWMRectangle * g;
  g = &c->size;
  return XCreateWindow(c->screen->xlib->display, c->screen->xlib->root,
    g->x, g->y, g->width, g->height, c->opt.border, CFP, CFP,
    NULL, CW_VM, &(XSetWindowAttributes){
      .event_mask = WA_EM,
      .override_redirect=true
    });
}
static inline void reparent_window(Display * d, Window parent, Window window)
{
  XAddToSaveSet(d, window);
  XReparentWindow(d, window, parent, 0, 0);
  XMapWindow(d, window);
}
static inline void reparent(struct JBWMClient *  c)
{
  JBWM_LOG("reparent()");
  jbwm_new_shaped_client(c);
  reparent_window(c->screen->xlib->display, c->parent = get_parent(c), c->window);
  // Required by wm-spec:
  jbwm_set_frame_extents(c);
}
// Allocate the client structure with some defaults set
static struct JBWMClient * get_JBWMClient(const Window w,
  struct JBWMScreen * s)
{
  struct JBWMClient *  c = calloc(1, sizeof(struct JBWMClient));
  c->screen=s;
  c->window=w;
  c->opt.border = 1;
  return c;
}
// Grab input and setup ewmh hints for client window
static void do_grabs(Display * d, const Window w)
{
  // jbwm_ewmh_set_allowed_actions must come before jbwm_grab_buttons.
  jbwm_ewmh_set_allowed_actions(d, w);
  XSelectInput(d, w, EnterWindowMask | PropertyChangeMask
    | ColormapChangeMask);
  // keys to grab:
  jbwm_grab_button(d, w, JBWM_KEYMASK_GRAB);//jbwm_get_grab_mask());
}
void jbwm_new_client(struct JBWMScreen * s,
  struct JBWMClient ** head_client,
  struct JBWMClient ** current_client,
  Window const w)
{
  struct JBWMClient * c = get_JBWMClient(w, s);
  JBWM_LOG("jbwm_new_client(..., w: %d)", (int)w);
  /* Prepend client.  */
  c->next=*head_client;
  *head_client=c;
  do_grabs(s->xlib->display, w);
  jbwm_set_client_geometry(c);
  jbwm_handle_mwm_hints(c);
  reparent(c);
  c->vdesk = get_vdesk(c);
  jbwm_snap_client(c, head_client);
  jbwm_restore_client(c);
  jbwm_select_client(c, current_client);
  if(c->screen->vdesk!=c->vdesk)
    jbwm_hide_client(c);
}
