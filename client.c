// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "client.h"

#include "ewmh.h"
#include "jbwmenv.h"
#include "log.h"
#include "screen.h"
#include "titlebar.h"
#include "util.h"

#include <assert.h>
#include <stdlib.h>
#include <X11/Xatom.h>

// Free result with XFree if not NULL
char * get_title(const Window w)
{
	XTextProperty tp;
	if (!XGetWMName(jbwm.dpy, w, &tp))
		  return NULL;
	return (char *)tp.value;
}

void client_to_vdesk(Client * c, const uint8_t d)
{
	LOG("client_to_vdesk");
	assert(c);
	const uint8_t p = c->vdesk;
	c->vdesk = d;
	// use switch_vdesk to validate d:
	c->vdesk = switch_vdesk(c->screen, d);
	switch_vdesk(c->screen, p);
}

/*
 * used all over the place.  return the client that has specified window as
 * either window or parent
 */
__attribute__ ((hot,pure))
Client *find_client(const Window w)
{
	Client *c=jbwm.head;
	while(c && c->parent != w && c->window !=w
#ifdef USE_TBAR
		&& c->titlebar != w
#endif//USE_TBAR
	     ) c=c->next;

	return c;
}

static void unselect_current(void)
{
	if(!jbwm.current) return;
	XSetWindowBorder(jbwm.dpy, jbwm.current->parent,
		jbwm.current->screen->pixels.bg);
	jbwm.current->opt.active = false;
}

void select_client(Client * c)
{
#ifdef MWM
	if(jbwm.current && jbwm.current->opt.modal)
		  return;
#endif//MWM
	c->opt.active = true;
	unselect_current();
	XInstallColormap(jbwm.dpy, c->cmap);
	XSetInputFocus(jbwm.dpy, c->window,
		RevertToPointerRoot, CurrentTime);
	XSetWindowBorder(jbwm.dpy, c->parent, c->opt.sticky
		? c->screen->pixels.fc : c->screen->pixels.fg);
	jbwm.current = c;
#ifdef EWMH
	XPROP(c->screen->root, ewmh[ACTIVE_WINDOW],
		XA_WINDOW, &(c->parent), 1);
#endif//EWMH
}

void stick(Client * c)
{
	LOG("stick");
	assert(c);
	c->vdesk = c->screen->vdesk;
	c->opt.sticky ^= true; // toggle
	select_client(c);
	update_titlebar(c);
}

// Returns 0 on failure.
static Status xmsg(const Window w, const Atom a, const long x)
{
	LOG("xmsg");
	return XSendEvent(jbwm.dpy, w, false, NoEventMask, &(XEvent){
		.xclient.type = ClientMessage, .xclient.window = w,
		.xclient.message_type = a, .xclient.format = 32,
		.xclient.data.l[0] = x, .xclient.data.l[1] = CurrentTime
	});
}

enum { CA_PROTOS, CA_DEL_WIN, CA_WM_STATE, CA_SZ };
static Atom client_atoms[CA_SZ];

static void setup_client_atoms(void)
{
	if(client_atoms[0]) return; // Already initialized
	char *names[]={"WM_PROTOCOLS", "WM_DELETE_WINDOW", "WM_STATE"};
	XInternAtoms(jbwm.dpy, names, CA_SZ, true, client_atoms);
}

__attribute__((nonnull))
void set_wm_state(Client * restrict c, const int state)
{
	LOG("set_wm_state(%d, %d)", (int)c->window, state);
	setup_client_atoms();
	XPROP(c->window, client_atoms[CA_WM_STATE], XA_CARDINAL, &state, 1);
}

__attribute__((nonnull))
static bool has_delete_proto(const Client * c)
{
	bool found=false;
	Atom *p;
	int i;
	if(XGetWMProtocols(jbwm.dpy, c->window, &p, &i)) {
		assert(p);
		while(i--)
			if((found=(p[i]==client_atoms[CA_DEL_WIN])))
				break;
		// Should be freed here, otherwise p has no alloc.
		XFree(p);
	}
	return found;
}

__attribute__((nonnull))
void send_wm_delete(const Client * restrict c)
{
	setup_client_atoms();
	if(has_delete_proto(c)) {
		assert(c->window);
		xmsg(c->window, client_atoms[CA_PROTOS],
			client_atoms[CA_DEL_WIN]);
	}
	else {
		assert(c->window);
		XKillClient(jbwm.dpy, c->window);
	}
}

