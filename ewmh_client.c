// Copyright 2020, Jeffrey E. Bedard
#include "ewmh_client.h"
#include "client.h"
#include "config.h"
#include "drag.h"
#include "ewmh.h"
#include "ewmh_wm_state.h"
#include "atom.h"
#include "select.h"
#include "util.h"
#include "vdesk.h"
#include "wm_state.h"
static void handle_moveresize(XClientMessageEvent * e)
{
  /* per wm-spec: fields come from e->data.l
     l[0]: gravity and flags, l[1]: x, l[2]: y, l[3]: w, l[4]: h
     l[0]: bits 0-7 idicate gravity,
     l[0]: bits 8-11 use x, y, width, height, respectively
     l[0]: bits 12-15 indicate the source,
     */
  enum {
    SRC_SHIFT = 12, SRC_MASK = 3, VM_SHIFT = 8, VM_MASK = 0xf,
    USER_ACTION = 2
  };
  const long * l = e->data.l;
  const uint8_t src = (l[0] >> SRC_SHIFT) & SRC_MASK;
  if (src != USER_ACTION)
    return;
  const uint32_t vm = (l[0] >> VM_SHIFT) & VM_MASK;
  XConfigureWindow(e->display, e->window, vm, &(XWindowChanges){
    .x = l[1], .y = l[2], .width = l[3], .height = l[4]});
  // use bit width truncation to mask for gravity:
  const uint8_t win_gravity = l[0];
  XChangeWindowAttributes(e->display, e->window, CWWinGravity,
    &(XSetWindowAttributes){.win_gravity = win_gravity});
}
// returns true if handled, false if not
static bool client_specific_message(XClientMessageEvent * e,
  struct JBWMClient * c, struct JBWMClient ** head_client,
  struct JBWMClient ** current_client, const Atom t)
{
  Display * d;
  d = e->display;
  jbwm_print_atom(d, t, __FILE__, __LINE__);
  if (t == jbwm_atoms[JBWM_NET_WM_DESKTOP])
    jbwm_set_client_vdesk(c, e->data.l[0]);
  // If user moves window (client-side title bars):
  else if (t == jbwm_atoms[JBWM_NET_WM_MOVERESIZE]) {
    XRaiseWindow(d, c->parent);
    jbwm_drag(c, head_client, false);
  } else if (t == jbwm_atoms[JBWM_NET_WM_STATE])
    jbwm_ewmh_handle_wm_state_changes(e, c);
  else if (t == jbwm_atoms[JBWM_NET_ACTIVE_WINDOW])
    jbwm_select_client(c, current_client);
  else if (t == jbwm_atoms[JBWM_NET_CLOSE_WINDOW])
    jbwm_send_wm_delete(c);
  else
    return false;
  return true;
}
#if defined(JBWM_DEBUG_EWMH_STATE) && defined(DEBUG)
static void debug_client_message(XClientMessageEvent * e)
{
  Display * d = e->display;
  const long * l = e->data.l;
  JBWM_LOG("jbwm_ewmh_client_message()");
  jbwm_print_atom(d, e->message_type, __FILE__, __LINE__);
  JBWM_LOG("\t\tl[0: %ld, 1: %ld, 2: %ld, 3: %ld, 4: %ld]",
    l[0], l[1], l[2], l[3], l[4]);
}
#else//!JBWM_DEBUG_EWMH_STATE||!DEBUG
#define debug_client_message(e)
#endif//JBWM_DEBUG_EWMH_STATE&&DEBUG
void jbwm_ewmh_handle_client_message(XClientMessageEvent * e,
  struct JBWMClient * c, struct JBWMClient ** head_client,
  struct JBWMClient ** current_client)
{
  const Atom t = e->message_type;
  debug_client_message(e);
  if(client_specific_message(e, c, head_client, current_client, t))
    return;
  if (t == jbwm_atoms[JBWM_NET_CURRENT_DESKTOP]) {
    jbwm_set_vdesk(c->screen, *head_client, e->data.l[0]);
  } else if (t == jbwm_atoms[JBWM_NET_MOVERESIZE_WINDOW]) {
    // If something else moves the window:
    handle_moveresize(e);
  }
}
