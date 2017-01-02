// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SCREEN_H
#define JBWM_SCREEN_H
#include "JBWMClient.h"
#include "JBWMScreen.h"
#include "jbwm.h" // for jbwm_get_screens()
inline struct JBWMScreen * jbwm_get_screen(struct JBWMClient * restrict c)
{
	return jbwm_get_screens() + c->screen;
}
inline Window jbwm_get_root(struct JBWMClient * restrict c)
{
	return jbwm_get_screen(c)->root;
}
void jbwm_hide_client(struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_move_resize(struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_restore_client(struct JBWMClient * restrict c)
	__attribute__((nonnull));
uint8_t jbwm_set_vdesk(struct JBWMScreen * s, uint8_t v)
	__attribute__((nonnull));
#endif /* JBWM_SCREEN_H */
