// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_EWMH_H
#define JBWM_EWMH_H
#ifdef JBWM_USE_EWMH
#include "JBWMClient.h"
void jbwm_ewmh_update_client_list(void);
void jbwm_ewmh_set_allowed_actions(const jbwm_window_t w);
void jbwm_ewmh_init_screen(struct JBWMScreen * restrict s)
	__attribute__((nonnull));
void jbwm_ewmh_init(void);
enum JBWMAtomIndex {
SUPPORTED, CURRENT_DESKTOP, NUMBER_OF_JBWM_MAX_DESKTOPS, DESKTOP_VIEWPORT,
    DESKTOP_GEOMETRY, SUPPORTING_WM_CHECK, ACTIVE_WINDOW,
    MOVERESIZE_WINDOW, CLOSE_WINDOW, CLIENT_LIST,
    VIRTUAL_ROOTS, CLIENT_LIST_STACKING, FRAME_EXTENTS,
// Atoms with "_NET_WM" prefix:
WM_ALLOWED_ACTIONS, WM_NAME, WM_DESKTOP, WM_MOVERESIZE, WM_PID,
    WM_WINDOW_TYPE,
// Atoms with "_NET_WM_ACTION" prefix:
WM_ACTION_MOVE, WM_ACTION_RESIZE, WM_ACTION_CLOSE,
    WM_ACTION_SHADE, WM_ACTION_FULLSCREEN,
    WM_ACTION_CHANGE_DESKTOP, WM_ACTION_ABOVE, WM_ACTION_BELOW,
    WM_ACTION_MAXIMIZE_HORZ, WM_ACTION_MAXIMIZE_VERT,
// Atoms with "_NET_WM_STATE" prefix:
WM_STATE, WM_STATE_STICKY, WM_STATE_MAXIMIZED_VERT,
    WM_STATE_MAXIMIZED_HORZ, WM_STATE_SHADED, WM_STATE_HIDDEN,
    WM_STATE_FULLSCREEN, WM_STATE_ABOVE, WM_STATE_BELOW,
    WM_STATE_FOCUSED,
// The following entry must be last:
    JBWM_EWMH_ATOMS_COUNT
};
extern Atom ewmh[];
#else//!JBWM_USE_EWMH
#define jbwm_ewmh_update_client_list()
#define jbwm_ewmh_set_allowed_actions(w)
#define jbwm_ewmh_init_screen(s)
#define jbwm_ewmh_init()
#endif//JBWM_USE_EWMH
#endif//!JBWM_EWMH_H
