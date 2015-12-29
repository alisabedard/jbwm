/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2015, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include "jbwm.h"
#include <unistd.h>

#ifdef EWMH

EWMHEnvironment ewmh;

void
ewmh_init()
{
	/* Initializing these elements with the preprocessor shortcut
		below, and following a variable naming convention directly
		corresponding to the atom name, allows further programatic
		access to the atoms defined and helps prevent typographic
		errors.  */
#define EA(atom) ewmh.supported[ewmh.count++]=ewmh.atom=XA("_NET_"#atom)
	EA(SUPPORTED); // Must be first!!!
	EA(CURRENT_DESKTOP);
	EA(NUMBER_OF_DESKTOPS);
	EA(DESKTOP_VIEWPORT);
	EA(DESKTOP_GEOMETRY);
	EA(SUPPORTING_WM_CHECK);
	EA(ACTIVE_WINDOW);
	EA(MOVERESIZE_WINDOW);
	EA(CLOSE_WINDOW);
	EA(CLIENT_LIST);
	EA(CLIENT_LIST_STACKING);
	EA(RESTACK_WINDOW);
	EA(REQUEST_FRAME_EXTENTS);
	EA(VIRTUAL_ROOTS);

	EA(WM_ALLOWED_ACTIONS);
	EA(WM_NAME);
	EA(WM_DESKTOP);
	EA(WM_MOVERESIZE);
	EA(WM_PID);
	EA(WM_MOVERESIZE_CANCEL);
	EA(WM_WINDOW_TYPE);
	EA(WM_STRUT_PARTIAL);

	EA(WM_ACTION_MOVE);
	EA(WM_ACTION_RESIZE);
	EA(WM_ACTION_CLOSE);
	EA(WM_ACTION_SHADE);
	EA(WM_ACTION_FULLSCREEN);
	EA(WM_ACTION_CHANGE_DESKTOP);
	EA(WM_ACTION_ABOVE);
	EA(WM_ACTION_BELOW);
	EA(WM_ACTION_MAXIMIZE_HORZ);
	EA(WM_ACTION_MAXIMIZE_VERT);

	EA(WM_STATE);
	EA(WM_STATE_STICKY);
	EA(WM_STATE_MAXIMIZED_VERT);
	EA(WM_STATE_MAXIMIZED_HORZ);
	EA(WM_STATE_SHADED);
	EA(WM_STATE_HIDDEN);
	EA(WM_STATE_FULLSCREEN);
	EA(WM_STATE_ABOVE);
	EA(WM_STATE_BELOW);
	EA(WM_STATE_DEMANDS_ATTENTION);
	EA(WM_STATE_FOCUSED);
	EA(WM_STATE_SKIP_PAGER);

	LOG("Added %d _NET atoms, %lu allowed", ewmh.count, 
		sizeof(ewmh.supported)/sizeof(Atom));
	// Quit if overflow, sanity check.  
	if(ewmh.count>sizeof(ewmh.supported)/sizeof(Atom))
		ERROR("BufferOverflow");
}

static void
set_desktop_viewport()
{
	const long vp[2]={0,0};
	XPROP(jbwm.X.screens->root, ewmh.DESKTOP_VIEWPORT, XA_CARDINAL, &vp, 2);
}

void
ewmh_update_client_list()
{
#define MAX_CLIENTS 1024
	// Prevent data from disappearing after return.
	static Window wl[MAX_CLIENTS];
	unsigned short count=0;
	for(Client *i=jbwm.head; i&&(count<MAX_CLIENTS); i=i->next)
	{
		LOG("count:%d", count);
		if(!i) break; // Prevent segfault
		wl[count++]=i->window;
	}
	XPROP(jbwm.X.screens->root, ewmh.CLIENT_LIST, XA_WINDOW, &wl, count);
	// FIXME: Does not correctly report stacking order.
	XPROP(jbwm.X.screens->root, ewmh.CLIENT_LIST_STACKING, XA_WINDOW, 
		&wl, count);
}


// Remove specified atom from WM_STATE
void
ewmh_remove_state(const Window w, const Atom state)
{
	Atom *a;
	unsigned long n;
	a=get_property(w, ewmh.WM_STATE, XA_ATOM, &n);
	if(!a) return;
	const unsigned long nitems=n;
	while(n--)
		if(a[n]==state)
			a[n]=0;
	XPROP(w, ewmh.WM_STATE, XA_ATOM, &a, nitems);
	XFree(a);
}

void
ewmh_add_state(const Window w, const Atom state)
{
	 XChangeProperty(D, w, ewmh.WM_STATE, XA_ATOM, 32, 
                        PropModePrepend, (unsigned char *)&state, 1);
}

static void
set_number_of_desktops(const Window r)
{
	const long num=DESKTOPS;
	XPROP(r, ewmh.NUMBER_OF_DESKTOPS, XA_CARDINAL, &num, 1);
	set_desktop_viewport();
	XPROP(r, ewmh.VIRTUAL_ROOTS, XA_WINDOW, &r, 1);
}

static void
handle_moveresize_window(Client *c, XClientMessageEvent *e)
{
	// Only do this if it came from direct user action 
	const int src = (e->data.l[0] >> 12) & 3;
	if (src == 2) 
	{
		c->size.x=e->data.l[1];
		c->size.y=e->data.l[2];
		c->size.width=e->data.l[3];
		c->size.height=e->data.l[4];
		moveresize(c);
	}
}

static void
handle_wm_state_changes(Client *c, XClientMessageEvent *e)
{
/* 	Reference, per wm-spec:
  window  = the respective client window
  message_type = _NET_WM_STATE
  format = 32
  data.l[0] = the action, as listed below
  data.l[1] = first property to alter
  data.l[2] = second property to alter
  data.l[3] = source indication:
        _NET_WM_STATE_REMOVE        0    remove/unset property 
        _NET_WM_STATE_ADD           1    add/set property 
        _NET_WM_STATE_TOGGLE        2    toggle property 
  other data.l[] elements = 0
*/     
                const Atom state=(Atom)(e->data.l[1]);
		const ubyte action=e->data.l[0];
                if(state==ewmh.WM_STATE_ABOVE && action) 
			XRaiseWindow(D, e->window);
                else if(state==ewmh.WM_STATE_BELOW && action) 
			XLowerWindow(D, e->window);
		else if(state==ewmh.WM_STATE_SKIP_PAGER && c)
			if(action) add_sticky(c);
			else remove_sticky(c);
                else if(state==ewmh.WM_STATE_FULLSCREEN && c)
		{
			// TODO: May need to add a FULLSCREEN flag
			const bool maxed=c->flags&JB_CLIENT_MAXIMIZED;
			switch(action)
			{
			case 0: //remove
				if(maxed) maximize(c); // toggle off
				break;
			case 1: //add
				if(!maxed) maximize(c); // toggle on
				break;
			case 2: //toggle
				maximize(c);
			}
		}
}

void
ewmh_client_message(XClientMessageEvent *e)
{
	const Atom t=e->message_type;
#ifdef DEBUG
        puts("----CLIENTMESSAGE----");
        print_atom(t, __LINE__);
        print_atom(e->data.l[0], __LINE__);
        print_atom(e->data.l[1], __LINE__);
        print_atom(e->data.l[2], __LINE__);
        print_atom(e->data.l[3], __LINE__);
#endif//DEBUG
        ScreenInfo *s = jbwm.X.screens;
        if(t==ewmh.CURRENT_DESKTOP) switch_vdesk(s, e->data.l[0]);
        Client *c=find_client(e->window);
	if(t==ewmh.WM_DESKTOP && c)
		client_to_vdesk(c, e->data.l[0]);
	else if(t==ewmh.DESKTOP_VIEWPORT)
		set_desktop_viewport();
	else if(t==ewmh.NUMBER_OF_DESKTOPS)
		set_number_of_desktops(s->root);
        else if(t==ewmh.ACTIVE_WINDOW && e->data.l[0]==2 && c)
		select_client(c);
	else if(t==ewmh.CLOSE_WINDOW && e->data.l[1]==2 && c)
		send_wm_delete(c);
        else if(t==ewmh.MOVERESIZE_WINDOW && c)
		handle_moveresize_window(c, e);
        else if(t==ewmh.WM_STATE)
		handle_wm_state_changes(c, e);
}

void
set_ewmh_allowed_actions(const Window w)
{
	Atom a[]={ewmh.WM_ALLOWED_ACTIONS, ewmh.WM_ACTION_MOVE,
		ewmh.WM_ACTION_RESIZE, ewmh.WM_ACTION_CLOSE,
		ewmh.WM_ACTION_SHADE, ewmh.WM_ACTION_FULLSCREEN,
		ewmh.WM_ACTION_CHANGE_DESKTOP, ewmh.WM_ACTION_ABOVE,
		ewmh.WM_ACTION_BELOW, ewmh.WM_ACTION_MAXIMIZE_HORZ,
		ewmh.WM_ACTION_MAXIMIZE_VERT};
	XPROP(w, a[0], XA_ATOM, &a, sizeof(a)/sizeof(Atom));
}      

void
setup_ewmh_for_screen(ScreenInfo *s)
{      
	const Window r=s->root;
	XPROP(r, ewmh.supported[0], XA_ATOM, ewmh.supported, ewmh.count);
        const unsigned long workarea[4] = { 0, 0, DisplayWidth(D, s->screen),
                DisplayHeight(D, s->screen)};
        XPROP(r, ewmh.DESKTOP_VIEWPORT, XA_CARDINAL, &workarea[0], 2);
        XPROP(r, ewmh.DESKTOP_GEOMETRY, XA_CARDINAL, &workarea[2], 2);
        const unsigned long vdesk = s->vdesk;
 	XPROP(r, ewmh.CURRENT_DESKTOP, XA_CARDINAL, &vdesk, 1);
        const unsigned char n=1;
        XPROP(r, ewmh.NUMBER_OF_DESKTOPS, XA_CARDINAL, &n, 1);
        XPROP(r, ewmh.WM_NAME, XA_STRING, "jbwm", 4);
        s->supporting=XCreateSimpleWindow(D, s->root, 0, 0, 1, 1, 0, 0, 0);
	XPROP(r, ewmh.SUPPORTING_WM_CHECK, XA_WINDOW, &(s->supporting), 1);
	XPROP(s->supporting, ewmh.SUPPORTING_WM_CHECK, XA_WINDOW,
		&(s->supporting), 1);
        XPROP(s->supporting, ewmh.WM_NAME, XA_STRING, "jbwm", 4);
        const pid_t pid=getpid();
        XPROP(s->supporting, ewmh.WM_PID, XA_CARDINAL, &pid, 1);
	// Set this to the root window until we have some clients.  
        XPROP(r, ewmh.CLIENT_LIST, XA_WINDOW, &r, 1);
}      

#endif//EWMH
