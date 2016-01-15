// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

static ScreenInfo *
find_screen(Window root)
{
	for(uint8_t i = 0; i < jbwm.X.num_screens; i++)
		if(jbwm.X.screens[i].root == root)
			return &jbwm.X.screens[i];
	return NULL;
}

static void
cleanup()
{
	LOG("cleanup()");
	jbwm.need_cleanup=0;
	Client *i;
	for(Client *c = jbwm.head; c; c = i)
	{
		i = c->next;
		if(c->flags & JB_REMOVE)
			remove_client(c);
		if(!i)
			return;
	}
}

static void
handle_unmap_event(XUnmapEvent * e)
{
	Client *c=find_client(e->window);
	if(!c) return;
	LOG("handle_unmap_event(e): %d ignores remaining", c->ignore_unmap);
	if(c->ignore_unmap<1)
	{
		LOG("!c->ignore_unmap");
		c->flags |= JB_REMOVE;
		jbwm.need_cleanup=true;
	}
	else c->ignore_unmap--;
}

#ifdef USE_CMAP
static void
handle_colormap_change(XColormapEvent * e)
{
	LOG("handle_colormap_change(e)");
	Client *c = find_client(e->window);
	if(c && e->new)
	{
		c->cmap = e->colormap;
		XInstallColormap(D, c->cmap);
	}
}
#endif /* USE_CMAP */

static void
handle_wm_hints(Client *c)
{
	LOG("handle_wm_hints");
	XWMHints *h=XGetWMHints(D, c->window);
	if(h->flags & XUrgencyHint)
	{
		switch_vdesk(c->screen, c->screen->vdesk);
		unhide(c);
		XRaiseWindow(D, c->parent);
	}
	XFree(h);
}

#ifdef DEBUG
void
print_atom(const Atom a, const unsigned int line)
{
        char *an=XGetAtomName(D, a);
        fprintf(stderr, "\t%s:%d %s(%lu)\n", __FILE__, line, an, a);
        XFree(an);
}       
#endif//DEBUG

static void
handle_property_change(XPropertyEvent * e)
{
	Client *c=find_client(e->window);
	if(!c) return;
	const Atom a=e->atom;
	if(a==XA_WM_HINTS) handle_wm_hints(c);
#ifdef USE_TBAR
	else if(a==XA_WM_NAME) update_titlebar(c);
	else moveresize(c); // Required to show titlebar on new client
#endif//USE_TBAR
}

static void
handle_enter_event(XCrossingEvent * e)
{
	Client *c=find_client(e->window);
	if(c) select_client(c);
}

#ifdef USE_TBAR
static void
handle_expose_event(XEvent * ev)
{
	// Ignore extra expose events
	if(ev->xexpose.count == 0)
	{
		LOG("handle_expose_event");
		/* xproperty was used instead of xexpose, previously.  */
		const Window w = ev->xexpose.window;
		Client *c=find_client(w);
		if(!c) return;
		if(w==c->titlebar) update_titlebar(c);
	}
}
#endif//USE_TBAR

static void
gravitate_border(Client *c, int bw) {
        int dx = 0, dy = 0;
        switch (c->size.win_gravity) {
        default:
        case NorthWestGravity:
                dx = bw;
                dy = bw;
                break;
        case NorthGravity:
                dy = bw;
                break;
        case NorthEastGravity:
                dx = -bw;
                dy = bw;
                break;
        case EastGravity:
                dx = -bw;
                break;
        case CenterGravity:
                break;
        case WestGravity:
                dx = bw;
                break;
        case SouthWestGravity:
                dx = bw;
                dy = -bw;
                break;
        case SouthGravity:
                dy = -bw;
                break;
        case SouthEastGravity:
                dx = -bw;
                dy = -bw;
                break;
        }
        if (c->size.x != 0 || c->size.width 
                != DisplayWidth(D, c->screen->screen)) 
                c->size.x += dx;
        if (c->size.y != 0 || c->size.height 
                != DisplayHeight(D, c->screen->screen))
                c->size.y += dy;
}      

void
do_window_changes(int value_mask, XWindowChanges *wc, Client *c, int gravity)
{
        if (gravity == 0)
                gravity = c->win_gravity_hint;
        c->size.win_gravity = gravity;
        if (value_mask & CWX) c->size.x = wc->x;
        if (value_mask & CWY) c->size.y = wc->y;
        if (value_mask & (CWWidth|CWHeight)) 
        {
                int dw = 0, dh = 0;
                if (!(value_mask & (CWX|CWY))) 
                        gravitate_border(c, -c->border);
                if (value_mask & CWWidth) 
                {
                        int neww = wc->width;
                        if (neww < c->size.min_width)
                                neww = c->size.min_width;
                        if (c->size.max_width && neww > c->size.max_width)
                                neww = c->size.max_width;
                        dw = neww - c->size.width;
                        c->size.width = neww;
                }
                if (value_mask & CWHeight) 
                {
                        int newh = wc->height;
                        if (newh < c->size.min_height)
                                newh = c->size.min_height;
                        if (c->size.max_height && newh > c->size.max_height)
                                newh = c->size.max_height;
                        dh = newh - c->size.height;
                        c->size.height = newh;
                }
                /* only apply position fixes if not being explicitly moved */
		if (!(value_mask & (CWX|CWY))) 
                {
                        switch (gravity) 
                        {
                        default:
                        case NorthWestGravity:
                                break;
                        case NorthGravity:
				c->size.x -= (dw / 2);
                                break;
                        case NorthEastGravity:
                                c->size.x -= dw;
                                break;
                        case WestGravity:
                                c->size.y -= (dh / 2);
                                break;
                        case CenterGravity:
                                c->size.x -= (dw / 2);
                                c->size.y -= (dh / 2);
                                break;
                        case EastGravity:
                                c->size.x -= dw;
                                c->size.y -= (dh / 2);
                                break;
                        case SouthWestGravity:
                                c->size.y -= dh;
                                break;
                        case SouthGravity:
                                c->size.x -= (dw / 2);
                                c->size.y -= dh;
                                break;
                        case SouthEastGravity:
                                c->size.x -= dw;
                                c->size.y -= dh;
                                break;
                        }
                        value_mask |= CWX|CWY;
                        gravitate_border(c, c->border);
                }
        }
        wc->x = c->size.x - c->border;
        wc->y = c->size.y - c->border;
        wc->border_width = c->border;
        XConfigureWindow(D, c->parent, value_mask, wc);
        XMoveResizeWindow(D, c->window, 0, 0, c->size.width, c->size.height);
#if 0
       if ((value_mask & (CWX|CWY)) && !(value_mask & (CWWidth|CWHeight))) 
                configure(c);
#endif//0
}

static void
handle_configure_request(XConfigureRequestEvent * e)
{
	LOG("handle_configure_request");
	XWindowChanges wc={.x=e->x, .y=e->y, .width=e->width, .height=e->height,
		.border_width=e->border_width, .sibling=e->above, 
		.stack_mode=e->detail};
	Client *c=find_client(e->window);
	if(c)
	{
                if (e->value_mask & CWStackMode && e->value_mask & CWSibling)
		{
                        Client *sibling = find_client(e->above);
                        if (sibling) 
			{
                                wc.sibling = sibling->parent;
                        }
                }
                do_window_changes(e->value_mask, &wc, c, 0);
        } 
	else 
	{
                LOG("XConfigureWindow(window=%lx, value_mask=%lx)", 
			(long unsigned int)e->window, e->value_mask);
                XConfigureWindow(D, e->window, e->value_mask, &wc);
        }
}

void
main_event_loop(void)
{
	XEvent ev;
head:
	XNextEvent(D, &ev);
	switch (ev.type)
	{
	case EnterNotify:
		handle_enter_event(&ev.xcrossing);
		break;
	case UnmapNotify:
		handle_unmap_event(&ev.xunmap);
		break;
	case PropertyNotify:
		handle_property_change(&ev.xproperty);
		break;
	case MapRequest:
		if(!find_client(ev.xmaprequest.window))
			make_new_client(ev.xmaprequest.window, 
			find_screen(ev.xmaprequest.parent));
		break;
	case KeyPress:
		jbwm_handle_key_event(&ev.xkey);
		break;
#ifdef USE_TBAR
	case Expose:
		handle_expose_event(&ev);
		break;
#endif
	case ButtonPress:
		jbwm_handle_button_event(&ev.xbutton);
		break;
	case ConfigureRequest:
		handle_configure_request(&ev.xconfigurerequest);
		break;
	case ConfigureNotify:
		if(!ev.xconfigure.override_redirect)
		{
			Client *c=find_client(ev.xconfigure.window);
			if(c) moveresize(c);
		}
		break;
#ifdef EWMH
	case CreateNotify:
	case DestroyNotify:
		ewmh_update_client_list();
		break;
#endif//EWMH
#ifdef USE_CMAP
	case ColormapNotify:
		handle_colormap_change(&ev.xcolormap);
		break;
#endif /* USE_CMAP */
#ifdef EWMH
	case ClientMessage:
		ewmh_client_message(&ev.xclient);
		break;
#endif//EWMH
#ifdef USE_SHAPE
	case ShapeNotify:
		LOG("ShapeNotify");
		set_shape(find_client(ev.xany.window));
		break;
#endif /* USE_SHAPE */
#if defined(DEBUG) && defined(XDEBUG)
	case MapNotify:
		LOG("MapNotify");
		break;
	case MappingNotify:
		LOG("MappingNotify");
		break;
	case KeyRelease:
		LOG("KeyRelease");
		break;
	case ReparentNotify:
		LOG("ReparentNotify");
		break;
	case ButtonRelease:
		LOG("ButtonRelease");
		break;
	default:
		LOG("Unhandled event (%d)", ev.type);
#endif//DEBUG&&XDEBUG
	}
	if(jbwm.need_cleanup)
		cleanup();
	goto head;
}

