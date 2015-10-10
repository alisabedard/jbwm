/* Copyright 2007-2011, Jeffrey E. Bedard */
#ifndef ARWM_EWMH_H
#ifdef USE_EWMH
void jbwm_ewmh_state(Client * c);
void jbwm_ewmh_type(Client * c);

#define ARWM_VWM_CHANGE_PROPERTY(c, property, state, n)\
        XChangeProperty(jbwm.X.dpy, c->window,\
		ARWM_ATOM_VWM_DESKTOP, property, 32,\
                PropModeReplace, state, n);

#define ARWM_UPDATE_NET_WM_DESKTOP(c)\
	ARWM_VWM_CHANGE_PROPERTY(c, XA_CARDINAL, (unsigned char *)&c->vdesk, 1);

void ewm_update_net_wm_state(Client * c);
#endif /* USE_EWMH */
#endif /* ARWM_EWMH_H */
