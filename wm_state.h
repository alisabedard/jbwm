// Copyright 2020, Alisa Bedard
#ifndef JBWM_WM_STATE_H
#define JBWM_WM_STATE_H
#include <X11/Xlib.h>
#include "JBWMClient.h"
// Return WM_STATE atom
Atom jbwm_get_wm_state(Display * d);
// Send WM_DELETE message to the client
void jbwm_send_wm_delete(struct JBWMClient * c)
__attribute__((nonnull));
// Set WM_STATE property
void jbwm_set_wm_state(const struct JBWMClient * c, int8_t state);
#endif//!JBWM_WM_STATE_H
