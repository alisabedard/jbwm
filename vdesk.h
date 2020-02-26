// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_VDESK_H
#define JBWM_VDESK_H
#include <X11/Xlib.h>
#include <stdint.h>
struct JBWMClient;
struct JBWMScreen;
/* Require the screen parameter here in case the head is NULL.  */
uint8_t jbwm_set_vdesk(struct JBWMScreen *s,
    struct JBWMClient *head, uint32_t v);
#endif//!JBWM_VDESK_H
