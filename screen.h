// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SCREEN_H
#define JBWM_SCREEN_H
#include "JBWMClient.h"
void jbwm_configure_client(struct JBWMClient * c)
	__attribute__((nonnull));
void jbwm_drag(struct JBWMClient * c, const bool resize)
	__attribute__((nonnull));
jbwm_window_t jbwm_get_root(struct JBWMClient * c);
struct JBWMScreen * jbwm_get_screen(struct JBWMClient * c);
void jbwm_hide_client(struct JBWMClient * c) __attribute__((nonnull));
void jbwm_move_resize(struct JBWMClient * c) __attribute__((nonnull));
uint8_t jbwm_set_vdesk(struct JBWMScreen * s, uint8_t v)
	__attribute__((nonnull));
void jbwm_restore_client(struct JBWMClient * c)
	__attribute__((nonnull));
#endif /* JBWM_SCREEN_H */
