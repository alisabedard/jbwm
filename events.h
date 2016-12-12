// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_EVENTS_H
#define JBWM_EVENTS_H
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_free_client(struct JBWMClient * restrict c) __attribute__((nonnull));
void jbwm_event_loop(Display * d) __attribute__((noreturn));
#endif /* ! JBWM_EVENTS_H */
