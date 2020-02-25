// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_EVENTS_H
#define JBWM_EVENTS_H
struct JBWMClient;
struct JBWMScreen; 
void jbwm_events_loop(struct JBWMScreen * s, struct JBWMClient
    **current_client) __attribute__((noreturn));
#endif /* ! JBWM_EVENTS_H */
