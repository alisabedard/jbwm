// Copyright 2020, Alisa Bedard <alisabedard@gmail.com>
#undef DEBUG
#include "ewmh_state.h"
#include "client.h"
#include "drag.h"
#include "ewmh.h"
#include "jbwm.h"
#include "atom.h"
#include "log.h"
#include "max.h"
#include "select.h"
#include "util.h"
#include "wm_state.h"
#include <X11/Xatom.h>
// Remove specified atom from WM_STATE
void jbwm_ewmh_remove_state(Display * display,
  const Window window, const Atom state)
{
  uint16_t n;
  const Atom wm_state = jbwm_atoms[JBWM_NET_WM_STATE];
  Atom *a = jbwm_get_property(display, window, wm_state, &n);
  if(a){ // found property
    uint16_t i;
    for(i=0;i<n;++i)
      if(a[i]==state)
        a[i]=0;
    XChangeProperty(display, window, wm_state, XA_ATOM, 32,
      PropModeReplace, (unsigned char *)a, n);
    XFree(a);
  }
}
static bool find_state(const Atom state, const int n, Atom * a)
{
  return n ? a[n] == state ? true : find_state(state, n - 1, a) : false;
}
bool jbwm_ewmh_get_state(Display * d,
  const Window w, const Atom state)
{
  uint16_t n;
  Atom *a = jbwm_get_property(d, w,
    jbwm_atoms[JBWM_NET_WM_STATE], &n);
  bool found = false;
  if (a) {
    found = find_state(state, n, a);
    XFree(a);
  }
  return found;
}
void jbwm_ewmh_add_state(Display * d, const Window w,
  Atom state)
{
  XChangeProperty(d, w, jbwm_atoms[JBWM_NET_WM_STATE],
    XA_ATOM, 32, PropModePrepend,
    (unsigned char *)&state, 1);
}

