// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_SCREEN_H
#define JBWM_SCREEN_H

#include "client_t.h"

enum {DESKTOPS=10};

void drag(Client * restrict c) __attribute__((nonnull));
void moveresize(Client * restrict c) __attribute__((nonnull));
void resize(Client * restrict c) __attribute__((nonnull));
uint8_t switch_vdesk(ScreenInfo * s, uint8_t v) __attribute__((nonnull));
void unhide(Client * restrict c) __attribute__((nonnull));

#endif /* JBWM_SCREEN_H */
