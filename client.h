
#ifndef ARWM_CLIENT_H
#define ARWM_CLIENT_H

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
	XSendEvent(arwm.X.dpy, c->window, False, StructureNotifyMask,\
		   (XEvent *)&(c->ce))

void 
send_wm_delete(Client * c, int kill_client);

#endif /* ARWM_CLIENT_H */

