// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H

#include "client_t.h"
#include <stdbool.h>
#include <X11/Xlib.h>


void client_to_vdesk(Client * restrict c, const uint8_t d);
Client *find_client(Window w);
// Free result with XFree if not NULL
char * get_title(const Window w);
void remove_client(Client * c);
void select_client(Client * c);
void send_wm_delete(const Client * restrict c);
void set_wm_state(Client * restrict c, const int state);
void stick(Client * c);

#endif /* JBWM_CLIENT_H */
