// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#include "jbwm.h"

static void
point(Client *c, const int x, const int y)
{
	XRaiseWindow(D, c->parent);
	XWarpPointer(D, None, c->window, 0, 0, 0, 0, x, y);
}

static void
keymv(Client * c, XKeyEvent * e, int *xy, int *wh, const int8_t sign)
{
	/* These operations invalid when maximized.  */
	if(c->flags & JB_MAXIMIZED) return;
	const int8_t d=sign*JBWM_RESIZE_INCREMENT;
	if((e->state & jbwm.keymasks.mod) && (*wh > 0))
	{
#ifdef USE_SHAPE
		if(c->flags&JB_SHAPED)
			goto move;
#endif//USE_SHAPE
		*wh += d;
	}
	else
#ifdef USE_SHAPE
move:
#endif//USE_SHAPE
		*xy += d;
#ifdef USE_SNAP
	snap_border(c);
#endif//USE_SNAP
	moveresize(c);
	point(c, 1, 1);
}

static void
handle_client_key_event(XKeyEvent * e, Client * c, KeySym key)
{
	XSizeHints *g = &(c->size);

	switch (key)
	{
	case KEY_LEFT:
		keymv(c, e, &(g->x), &(g->width), -1);
		break;
	case KEY_DOWN:
		keymv(c, e, &(g->y), &(g->height), 1);
		break;
	case KEY_UP:
		keymv(c, e, &(g->y), &(g->height), -1);
		break;
	case KEY_RIGHT:
		keymv(c, e, &(g->x), &(g->width), 1);
		break;
	case KEY_KILL:
		send_wm_delete(c);
		break;
	case KEY_LOWER:
	case KEY_ALTLOWER:
		XLowerWindow(D, c->parent);
		break;
	case KEY_RAISE:
		XRaiseWindow(D, c->parent);
		break;
	case KEY_MAX:
		maximize(c);
		break;
	case KEY_STICK:
		stick(c);
		break;
	case KEY_MOVE:
		drag(c);
		break;
#ifdef USE_TBAR
	case KEY_SHADE:
		shade(c);
#endif//USE_TBAR
	}
}

static void
next(void)
{
	LOG("next()");
	Client *c=jbwm.current;
	do
	{
		if(c)
		{
			c=c->next;
			if(!c&&!jbwm.current)
				return;
		}
		if(!c)
			c=jbwm.head;
		if(!c)
			return;
		if(c==jbwm.current)
			return;
	} while(c->vdesk != c->screen->vdesk);
	if(!c) return;
	unhide(c);
	select_client(c);
	point(c, 0, 0);
	point(c, c->size.width, c->size.height);
}

static void
cond_client_to_desk(Client *c, ScreenInfo *s, const uint8_t d, const bool mod)
{
	LOG("mod: %d, c valid? %d\n", mod, c?1:0);
	if(mod&&c)
		client_to_vdesk(c, d);
	else 
		switch_vdesk(s, d);
}

static void
spawn(const char *cmd)
{
	const int r=system(cmd);
	if(WIFEXITED(r) && WEXITSTATUS(r))
		ERROR(cmd);
}

void
jbwm_handle_key_event(XKeyEvent * e)
{
	KeySym key = XLookupKeysym(e, 0);
	Client *c=jbwm.current;
	ScreenInfo *s = c ? c->screen : jbwm.X.screens;
	const bool mod=e->state&jbwm.keymasks.mod;
	bool zero_desk=false;
	uint8_t new_desk;
	switch (key)
	{
	case KEY_NEW:
		spawn(TERMINAL_CMD);
		break;
	case KEY_QUIT:
		exit(0);
	case KEY_NEXT:
		next();
		break;
	case XK_0:
		zero_desk=true;
	case XK_1:
	case XK_2:
	case XK_3:
	case XK_4:
	case XK_5:
	case XK_6:
	case XK_7:
	case XK_8:
	case XK_9:
		// First desktop 0, per wm-spec
		new_desk=zero_desk?10:key-XK_1;
		cond_client_to_desk(c, s, new_desk, mod);
		break;
	case KEY_PREVDESK:
		new_desk=s->vdesk-1;
		cond_client_to_desk(c, s, new_desk, mod);
		break;
	case KEY_NEXTDESK:
		new_desk=s->vdesk+1;
		cond_client_to_desk(c, s, new_desk, mod);
		break;
	default:
		if(jbwm.current)
			handle_client_key_event(e, jbwm.current, key);
	}
}

static void
grab(ScreenInfo * s, KeySym * ks, const unsigned int mask)
{
	for(;*ks; ks++)
	{
		XGrabKey(D, XKeysymToKeycode(jbwm.X.dpy, *ks), 
			jbwm.keymasks.grab| mask, s->root, true, GrabModeAsync, 
			GrabModeAsync); 
	}
}

void
grab_keys_for_screen(ScreenInfo *restrict s)
{
	KeySym keys[] = JBWM_KEYS_TO_GRAB;
	grab(s, keys, 0);
	KeySym mod_keys[] = JBWM_ALT_KEYS_TO_GRAB;
	grab(s, mod_keys, jbwm.keymasks.mod);
}

