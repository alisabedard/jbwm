// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_VDESK_H
#define JBWM_VDESK_H
#include <X11/Xlib.h>
#include <stdint.h>
struct JBWMScreen;
uint8_t jbwm_set_vdesk(Display * d, struct JBWMScreen * s, uint8_t v);
#endif//!JBWM_VDESK_H
