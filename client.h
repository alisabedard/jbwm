/* jbwm, Copyright 2016 Jeffrey E. Bedard */
#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H

void
shade(Client *c);

void
client_to_vdesk(Client *c, const ubyte d);

void
configure(Client *c);

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
