// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_SCREEN_H
#define JBWM_SCREEN_H

// Used by vdesk switching and EWMH
#define DESKTOPS 10

typedef struct
{
	int x, y;
} Position;

void drag(Client * c);
void moveresize(Client * c);
void show_info(Client * c, KeySym key);
void sweep(Client * c);
void hide(Client * c);
void unhide(Client * c);
uint8_t switch_vdesk(ScreenInfo * s, const uint8_t v);
void grab_keys_for_screen(ScreenInfo * s);
void maximize(Client * c);
void get_mouse_position(Window w, Position *p);

#endif /* JBWM_SCREEN_H */
