// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_EVENTS_H
#define JBWM_EVENTS_H
#include <X11/Xlib.h>
struct JBWMClient;
// Allow future clients with the same window ID:
void jbwm_events_clear_last_window(void);
void jbwm_event_loop(Display * d) __attribute__((noreturn));
#endif /* ! JBWM_EVENTS_H */
