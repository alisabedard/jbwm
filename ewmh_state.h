// Copyright 2020, Alisa Bedard
#ifndef JBWM_EWMH_STATE_H
#define JBWM_EWMH_STATE_H
#include <X11/Xlib.h>
#include <stdbool.h>
struct JBWMClient;
void jbwm_ewmh_add_state(Display * d, const Window w, Atom state);
bool jbwm_ewmh_get_state(Display * d, const Window w, const Atom state);
void jbwm_ewmh_remove_state(Display * d, const Window w, const Window state);
#endif//JBWM_EWMH_STATE_H
