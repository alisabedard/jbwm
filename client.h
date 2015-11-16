
#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H

/*
 * This is a macro version of set_client_ce, the function that is
 * most-called, according to gprof.
 */
#define SET_CLIENT_CE(c) \
	c->ce.event=c->window;\
	c->ce.x=c->geometry.x;\
	c->ce.y=c->geometry.y;\
	c->ce.width=c->geometry.width;\
	c->ce.height=c->geometry.height;

/*
 * This is the send_config macro version which uses already-known
 * XConfigureEvent data.
 */
#define SEND_CONFIG(c)\
	XSendEvent(jbwm.X.dpy, c->window, false, StructureNotifyMask,\
		   (XEvent *)&(c->ce))

void send_wm_delete(Client * c);

Client *find_client(Window w);
void gravitate_client(Client * c, int sign);
void select_client(Client * c);

void fix_client(Client * c);

void remove_client(Client * c);
void send_config(Client * c);
void set_wm_state(Client * c, int state);
bool is_shaped(Client * c);
void set_shape(Client * c);

#endif /* JBWM_CLIENT_H */
