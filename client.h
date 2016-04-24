// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H

#include <stdbool.h>
#include <X11/Xlib.h>
#include "client_t.h"

// Free result with XFree if not NULL
char * get_title(const Window w);

#ifdef USE_TBAR
void shade(Client * restrict c);
#else
#define shade(c)
#endif

void client_to_vdesk(Client * restrict c, const uint8_t d);

Status xmsg(const Window w, const Atom a, const long x);

void send_wm_delete(const Client * restrict c);

Client *find_client(Window w);

void select_client(Client * c);

void stick(Client * c);

void remove_client(Client * c);

void send_config(Client * c);

void set_wm_state(Client * restrict c, const int state);

#endif /* JBWM_CLIENT_H */
