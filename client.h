/* jbwm, Copyright 2015 Jeffrey E. Bedard */
#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H

void
configure(Client *c);

void
send_configure(Client *c);

void 
xmsg(Window w, Atom a, long x);

void 
send_wm_delete(Client * c);

Client *
find_client(Window w);

void 
gravitate_client(Client * c, int sign);

void 
select_client(Client * c);

void 
fix_client(Client * c);

void 
remove_client(Client * c);

void 
send_config(Client * c);

void 
set_wm_state(Client * c, int state);

bool 
is_shaped(Client * c);

void 
set_shape(Client * c);

#endif /* JBWM_CLIENT_H */
