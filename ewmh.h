/* Copyright 2007, Jeffrey E. Bedard */
#ifndef ARWM_EWMH_H

void
arwm_setup_ewmh_hints(void);

void
arwm_handle_ewmh_hints(Client * c);

void
arwm_handle_net_wm_state_hints(Client * c);

#define ARWM_VWM_CHANGE_PROPERTY(c, property, state, n)\
        XChangeProperty(arwm.X.dpy, c->window,\
		arwm.atoms.vwm.desktop, property, 32,\
                PropModeReplace, state, n);

#define ARWM_UPDATE_NET_WM_DESKTOP(c)\
	ARWM_VWM_CHANGE_PROPERTY(c, XA_CARDINAL, (unsigned char *)&c->vdesk, 1);

void
ewm_update_net_wm_state(Client * c);

#endif /* ARWM_EWMH_H */

