// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_EWMH_STATE_H
#define JBWM_EWMH_STATE_H
#ifdef JBWM_USE_EWMH
#include <X11/Xlib.h>
#include <stdbool.h>
struct JBWMClient;
void jbwm_ewmh_add_state(Display * d, const Window w, Atom state);
bool jbwm_ewmh_get_state(Display * d, const Window w, const Atom state);
void jbwm_ewmh_remove_state(Display * d, const Window w, const Window state);
#else//!JBWM_USE_EWMH
#define jbwm_ewmh_add_state(d, w, s)
#define jbwm_ewmh_get_state(d, w, s)
#define jbwm_ewmh_remove_state(d, w, s)
#endif//JBWM_USE_EWMH
#endif//JBWM_EWMH_STATE_H
