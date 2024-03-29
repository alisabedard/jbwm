// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_EWMH_H
#define JBWM_EWMH_H
#include <stdint.h>
#include <X11/Xlib.h>
struct JBWMClient;
struct JBWMScreen;
void jbwm_ewmh_set_allowed_actions(Display * d,
  const Window w) __attribute__((nonnull));
void jbwm_ewmh_init_screen(Display * d, struct JBWMScreen * s)
__attribute__((nonnull));
void jbwm_set_frame_extents(struct JBWMClient * c);
#endif//!JBWM_EWMH_H
