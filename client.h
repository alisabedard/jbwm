// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H

#include "JBWMClient.h"

void client_to_vdesk(struct JBWMClient * restrict c, const uint8_t d)
	__attribute((nonnull));

struct JBWMClient *find_client(jbwm_window_t w)
	__attribute__((hot,pure));

// Free result with XFree if not NULL
char * get_title(const jbwm_window_t w)
	__attribute__((pure));

void select_client(struct JBWMClient * c);

void send_wm_delete(const struct JBWMClient * restrict c)
	__attribute__((nonnull));

// Set WM_STATE property
void set_wm_state(struct JBWMClient * c, const int8_t state);

// Return WM_STATE atom
jbwm_atom_t get_wm_state(void);

void stick(struct JBWMClient * c)
	__attribute__((nonnull,cold));

#endif /* JBWM_CLIENT_H */
