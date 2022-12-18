// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020,Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015,Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "ewmh.h"
#include "atom.h"
#include "config.h"
#include "JBWMClient.h"
#include "log.h"
#include "macros.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xatom.h>
void jbwm_ewmh_set_allowed_actions(Display * d,
  Window const w)
{
  Atom a[]={
    jbwm_atoms[JBWM_NET_WM_ACTION_MOVE],
    jbwm_atoms[JBWM_NET_WM_ACTION_RESIZE],
    jbwm_atoms[JBWM_NET_WM_ACTION_CLOSE],
    jbwm_atoms[JBWM_NET_WM_ACTION_SHADE],
    jbwm_atoms[JBWM_NET_WM_ACTION_FULLSCREEN],
    jbwm_atoms[JBWM_NET_WM_ACTION_CHANGE_DESKTOP],
    jbwm_atoms[JBWM_NET_WM_ACTION_ABOVE],
    jbwm_atoms[JBWM_NET_WM_ACTION_BELOW],
    jbwm_atoms[JBWM_NET_WM_ACTION_MAXIMIZE_HORZ],
    jbwm_atoms[JBWM_NET_WM_ACTION_MAXIMIZE_VERT],
  };
  XChangeProperty(d,w,a[0],XA_ATOM, 32, PropModeReplace,
    (unsigned char *)&a,sizeof(a) / sizeof(Atom));
}
static void init_desktops(Display * d,struct JBWMScreen * s)
{
  int32_t data[2];
  Window root=s->xlib->root;
  data[0]=s->xlib->width;
  data[1]=s->xlib->height;
  XChangeProperty(d, root, jbwm_atoms[JBWM_NET_DESKTOP_GEOMETRY],
    XA_CARDINAL, 32, PropModeReplace, (unsigned char*)data, 2);
  data[0]=data[1]=0;
  XChangeProperty(d, root, jbwm_atoms[JBWM_NET_DESKTOP_VIEWPORT],
    XA_CARDINAL, 32, PropModeReplace, (unsigned char*)data, 2);
  data[0]=JBWM_NUMBER_OF_DESKTOPS;
  XChangeProperty(d, root, jbwm_atoms[JBWM_NET_NUMBER_OF_DESKTOPS],
    XA_CARDINAL, 32, PropModeReplace, (unsigned char*)data, 1);
  XChangeProperty(d, root, jbwm_atoms[JBWM_NET_CURRENT_DESKTOP],
    XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&s->vdesk, 1);
  XChangeProperty(d, root, jbwm_atoms[JBWM_NET_VIRTUAL_ROOTS],
    XA_WINDOW, 32, PropModeReplace, (unsigned char*)&root, 1);
}
static Window init_supporting(Display * d,Window const r)
{
  Window w;
  w=XCreateSimpleWindow(d,r,0,0,1,1,0,0,0);
  XChangeProperty(d, r, jbwm_atoms[JBWM_NET_SUPPORTING_WM_CHECK],
    XA_WINDOW, 32, PropModeReplace, (unsigned char *)&w, 1);
  XChangeProperty(d, w, jbwm_atoms[JBWM_NET_SUPPORTING_WM_CHECK],
    XA_WINDOW, 32, PropModeReplace, (unsigned char *)&w, 1);
  XChangeProperty(d, w, jbwm_atoms[JBWM_NET_WM_PID], XA_CARDINAL,
    32, PropModeReplace, (unsigned char *)&(pid_t){getpid()},1);
  XChangeProperty(d, w, jbwm_atoms[JBWM_NET_WM_NAME], XA_STRING,
    8, PropModeReplace, (unsigned char *)JBWM_NAME, sizeof(JBWM_NAME));
  return w;
}
void jbwm_ewmh_init_screen(Display * d, struct JBWMScreen * s)
{
  Window *r;
  /* Use a pointer to the original screen data to prevent
   * the pointers passed as property data from being invalid
   * once this function looses scope.  */
  r=&s->xlib->root;
  /* Set this to the root window until we have some clients.
     Point to original screen data. */
  XChangeProperty(d, *r, jbwm_atoms[JBWM_NET_CLIENT_LIST], XA_WINDOW,
    32, PropModeReplace, (unsigned char *)r, 1);
  XChangeProperty(d,*r,jbwm_atoms[JBWM_NET_SUPPORTED],
    XA_ATOM, 32, PropModeReplace, (unsigned char *)&jbwm_atoms,
    JBWM_ATOM_COUNT);
  XChangeProperty(d,*r,jbwm_atoms[JBWM_NET_WM_NAME],XA_STRING,
    8, PropModeReplace, (unsigned char *)JBWM_NAME, sizeof(JBWM_NAME));
  init_desktops(d,s);
  s->supporting=init_supporting(d,*r);
}
// Required by wm-spec:
void jbwm_set_frame_extents(struct JBWMClient * c)
{
  static uint32_t f[4];
  Atom a;
  JBWM_LOG("jbwm_set_frame_extents()");
  // Fields: left,right,top,bottom
  f[0]=f[1]=f[2]=f[3]=c->opt.border;
  if (!c->opt.no_title_bar)
    f[2] += c->screen->font_height;
  a=jbwm_atoms[JBWM_NET_FRAME_EXTENTS];
  XChangeProperty(c->screen->xlib->display, c->parent, a, XA_CARDINAL, 32,
    PropModeReplace, (unsigned char *)f, 4);
}
