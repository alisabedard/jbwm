// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H

void
shade(Client *c);

void
client_to_vdesk(Client *c, const uint8_t d);

void 
xmsg(Window w, Atom a, long x);

void 
send_wm_delete(Client * c);

Client *
find_client(Window w);

void 
select_client(Client * c);

void 
stick(Client * c);

void 
remove_client(Client * c);

void 
send_config(Client * c);

void 
set_wm_state(Client * c, int state);

#ifdef USE_SHAPE
bool
set_shape(Client * c);
#endif//USE_SHAPE

#endif /* JBWM_CLIENT_H */
