// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

#ifdef USE_TBAR
void
shade(Client * c)
{
	LOG("shade");
	assert(c);
	// Honor !MWM_FUNC_MINIMIZE
	if(c->flags & JB_NO_MIN)
		return;
        /* This implements window shading, a substitute 
           for iconification.  */
        if(c->flags & JB_SHADED)
        { // Unshade
                c->size.height=c->shade_height;
                c->flags &= ~JB_SHADED;
                XMapWindow(D, c->window);
                moveresize(c);
		set_wm_state(c, NormalState);
#ifdef EWMH
		ewmh_remove_state(c->window, ewmh.WM_STATE_SHADED);
#endif//EWMH
        }
        else // Shade the client
        {
                c->shade_height = c->size.height;
                c->ignore_unmap++;
                XUnmapWindow(D, c->window);
                c->size.height=0;
                c->flags |= JB_SHADED;
		set_wm_state(c, IconicState);
#ifdef EWMH
		ewmh_add_state(c->window, ewmh.WM_STATE_SHADED);
#endif//EWMH
                select_client(c);
	}
}
#endif//USE_TBAR

void
client_to_vdesk(Client *c, const uint8_t d)
{
	LOG("client_to_vdesk");
	assert(c);
	c->vdesk=d;
        c->vdesk=switch_vdesk(c->screen, d);
}

/*
 * used all over the place.  return the client that has specified window as
 * either window or parent
 */
__attribute__((hot))
Client *
find_client(const Window w)
{
	Client *c=jbwm.head;
#ifdef USE_TBAR
	while(c&&c->parent!=w&&c->window!=w&&c->titlebar!=w)
#else//!USE_TBAR
	while(c&&c->parent!=w&&c->window!=w)
#endif//USE_TBAR
		c=c->next;
	return c;
}
void
set_wm_state(Client * c, int state)
{
	assert(c);
	LOG("set_wm_state(%d, %d)", (int)c->window, state);
	XPROP(c->window, XA("WM_STATE"), XA_CARDINAL, &state, 1);
}

void
select_client(Client * c)
{
	LOG("select_client");
	if(jbwm.current)
	{
		XSetWindowBorder(D, jbwm.current->parent,
			jbwm.current->screen->bg.pixel);
		jbwm.current->flags &= ~JB_ACTIVE;
	}
	if(c)
	{
		c->flags |= JB_ACTIVE;
		XSetWindowBorder(D, c->parent, (c->flags&JB_STICKY)
			? c->screen->fg.pixel : c->screen->fc.pixel);
#ifdef USE_CMAP
		XInstallColormap(D, c->cmap);
#endif /* USE_CMAP */
		XSetInputFocus(D, c->window, RevertToPointerRoot, CurrentTime);
#ifdef EWMH
		XPROP(c->screen->root, ewmh.ACTIVE_WINDOW, XA_WINDOW,
			&(c->window), 1);
#endif//EWMH
	}
	jbwm.current = c;
}

void
stick(Client * c)
{
	LOG("stick");
	c->vdesk=c->screen->vdesk;
	c->flags^=JB_STICKY; // toggle
	select_client(c);
#ifdef USE_TBAR
	update_titlebar(c);
#endif//USE_TBAR
}

static inline void
relink_window_list(Client * c)
{
	LOG("relink_window_list");
	if(jbwm.head == c) jbwm.head = c->next;
	else for(Client *p = jbwm.head; p && p->next; p = p->next)
		if(p->next == c)
		{
			p->next = c->next;
			return;
		}
}

static void
unparent_window(Client * c)
{
	LOG("unparent_window");
	if(!c->parent) return;
	XReparentWindow(D, c->window, c->screen->root, c->size.x, c->size.y);
	XRemoveFromSaveSet(D, c->window);
	if(c->parent) XDestroyWindow(D, c->parent);
	c->parent=0;
}

void
remove_client(Client * c)
{
	LOG("remove_client");
	if(c->flags&JB_REMOVE)
	{
#ifdef EWMH
		XDeleteProperty(D, c->window, ewmh.WM_DESKTOP);
		XDeleteProperty(D, c->window, ewmh.WM_STATE);
#endif//EWMH
		set_wm_state(c, WithdrawnState);
	}
#ifdef EWMH
	else XDeleteProperty(D, c->window, ewmh.WM_ALLOWED_ACTIONS);
#endif//EWMH
	unparent_window(c);
	relink_window_list(c);
	if(jbwm.current==c) jbwm.current=NULL;
	free(c);
}

void
xmsg(Window w, Atom a, long x)
{
	XLOG("xmsg");
	XEvent ev = {.xclient.type=ClientMessage, .xclient.window=w, 
		.xclient.message_type=a, .xclient.format=32, 
		.xclient.data.l[0]=x, .xclient.data.l[1]=CurrentTime};
	XSendEvent(D, w, false, NoEventMask, &ev);
}

void
send_wm_delete(Client * c)
{
	LOG("send_wm_delete");
	xmsg(c->window, XA("WM_PROTOCOLS"), XA("WM_DELETE_WINDOW"));
}

#ifdef USE_SHAPE
bool
set_shape(Client * c)
{
	XLOG("set_shape");
	/* Validate inputs:  Make sure that the SHAPE extension is available, 
		and make sure that C is initialized.  */
	if(c && (c->flags & JB_SHAPED))
	{
		XShapeCombineShape(D, c->parent, ShapeBounding, 0, 0, 
			c->window, ShapeBounding, ShapeSet);
		return true;
	}
	return false;
}
#endif//USE_SHAPE
