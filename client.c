// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include <assert.h>
#include <stdlib.h>
#ifdef USE_SHAPE
#include <X11/extensions/shape.h>
#endif//USE_SHAPE
#include "atoms.h"
#include "client.h"
#include "ewmh.h"
#include "jbwm.h"
#include "log.h"
#include "screen.h"
#include "titlebar.h"

// Free result with XFree if not NULL
char * get_title(const Window w)
{
	XTextProperty tp;
	if (!XGetWMName(jbwm.dpy, w, &tp)) 
		  return NULL;
	return (char *)tp.value;
}

#ifdef USE_TBAR
void shade(Client * restrict c)
{
	LOG("shade");
	assert(c);

	// Honor !MWM_FUNC_MINIMIZE
	if (c->flags & JB_NO_MIN)
		return;

	/* This implements window shading, a substitute
	   for iconification.  */
	if (c->flags & JB_SHADED) {
		// Unshade
		c->size.height = c->shade_height;
		c->flags &= ~JB_SHADED;
		XMapWindow(jbwm.dpy, c->window);
		moveresize(c);
		set_wm_state(c, NormalState);
		ewmh_remove_state(c->window, ewmh.WM_STATE_SHADED);
	} else {		// Shade the client
		c->shade_height = c->size.height;
		c->ignore_unmap++;
		XUnmapWindow(jbwm.dpy, c->window);
		c->size.height = 0;
		c->flags |= JB_SHADED;
		set_wm_state(c, IconicState);
		ewmh_add_state(c->window, ewmh.WM_STATE_SHADED);
		select_client(c);
	}
}
#endif//USE_TBAR

void client_to_vdesk(Client * restrict c, const uint8_t d)
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
__attribute__ ((hot))
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

#define CA_SZ 3
static Atom client_atoms[CA_SZ];
#define I_PROTOS 0
#define I_DEL_WIN 1
#define I_WM_STATE 2

static void setup_client_atoms()
{
	if(client_atoms[0])
		  return; // Already initialized
	char *names[]={"WM_PROTOCOLS", "WM_DELETE_WINDOW", "WM_STATE"};
	XInternAtoms(jbwm.dpy, names, CA_SZ, true, client_atoms);
}

void set_wm_state(Client * restrict c, const int state)
{
	assert(c);
	LOG("set_wm_state(%d, %d)", (int)c->window, state);
	setup_client_atoms();
	XPROP(c->window, client_atoms[I_WM_STATE], XA_CARDINAL, &state, 1);
}

static void unselect_current(void)
{
	if(!jbwm.current) return;
	XSetWindowBorder(jbwm.dpy, jbwm.current->parent,
		jbwm.current->screen->pixels.bg);
	jbwm.current->flags ^= JB_ACTIVE;
}

void select_client(Client * restrict c)
{
	c->flags |= JB_ACTIVE;
	unselect_current();
	XInstallColormap(jbwm.dpy, c->cmap);
	XSetInputFocus(jbwm.dpy, c->window, RevertToPointerRoot, CurrentTime);
	XSetWindowBorder(jbwm.dpy, c->parent, c->flags & JB_STICKY 
		? c->screen->pixels.fc : c->screen->pixels.fg);
	jbwm.current=c;
#ifdef EWMH
	XPROP(c->screen->root, ewmh.ACTIVE_WINDOW, XA_WINDOW, &(c->parent), 1);
#endif//EWMH
}

void stick(Client * c)
{
	LOG("stick");
	assert(c);
	c->vdesk = c->screen->vdesk;
	c->flags ^= JB_STICKY;	// toggle
	select_client(c);
#ifdef USE_TBAR
	update_titlebar(c);
#endif//USE_TBAR
}

static void relink_window_list(Client * c)
{
	LOG("relink_window_list");

	if (jbwm.head == c)
		jbwm.head = c->next;
	else
		for (Client * p = jbwm.head; p && p->next; p = p->next)
			if (p->next == c) {
				p->next = c->next;
				return;
			}
}

static void unparent_window(Client * restrict c)
{
	LOG("unparent_window");
	XReparentWindow(jbwm.dpy, c->window, c->screen->root, c->size.x, c->size.y);
	XRemoveFromSaveSet(jbwm.dpy, c->window);

	if (c->parent)
		XDestroyWindow(jbwm.dpy, c->parent);

	c->parent = 0;
}

void remove_client(Client * c)
{
	LOG("remove_client");
	assert(c);
	if (c->flags & JB_REMOVE) {
#ifdef EWMH
		XDeleteProperty(jbwm.dpy, c->window, ewmh.WM_DESKTOP);
		XDeleteProperty(jbwm.dpy, c->window, ewmh.WM_STATE);
#endif//EWMH
		set_wm_state(c, WithdrawnState);
	}
#ifdef EWMH
	else
		XDeleteProperty(jbwm.dpy, c->window, ewmh.WM_ALLOWED_ACTIONS);

#endif//EWMH
	unparent_window(c);
	relink_window_list(c);

	if (jbwm.current == c)
		jbwm.current = NULL;

	free(c);
}

// Returns 0 on failure.  
Status xmsg(const Window w, const Atom a, const long x)
{
	XLOG("xmsg");
	return XSendEvent(jbwm.dpy, w, false, NoEventMask, &(XEvent){
		.xclient.type = ClientMessage,.xclient.window = w,
		.xclient.message_type = a,.xclient.format = 32,
		.xclient.data.l[0] = x,.xclient.data.l[1] = CurrentTime
	});
}

static bool has_delete_proto(const Client * restrict c)
{
	bool found=false;
	Atom *p;
	int i;
	if(XGetWMProtocols(jbwm.dpy, c->window, &p, &i)) {
		assert(p);
		while(i--)
			if((found=(p[i]==client_atoms[I_DEL_WIN])))
				break;
		// Should be freed here, otherwise p has no alloc.  
		XFree(p);
	}
	return found;
}

void send_wm_delete(const Client * restrict c)
{
	assert(c);
	setup_client_atoms();
	if(has_delete_proto(c)) {
		assert(c->window);
		xmsg(c->window, client_atoms[I_PROTOS],
			client_atoms[I_DEL_WIN]);
	}
	else {
		assert(c->window);
		XKillClient(jbwm.dpy, c->window);
	}
}

#ifdef USE_SHAPE
bool set_shape(Client * restrict c)
{
	XLOG("set_shape");
	assert(c);
	/* Validate inputs:  Make sure that the SHAPE extension is available,
	   and make sure that C is initialized.  */
	if (c && (c->flags & JB_SHAPED)) {
		XShapeCombineShape(jbwm.dpy, c->parent, ShapeBounding, 0, 0,
				   c->window, ShapeBounding, ShapeSet);
		return true;
	}

	return false;
}
#endif//USE_SHAPE
