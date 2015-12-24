/* Copyright 2007-2011, Jeffrey E. Bedard */
#ifndef JBWM_SCREEN_H
#define JBWM_SCREEN_H

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
void switch_vdesk(ScreenInfo * s, const ubyte v);
void grab_keys_for_screen(ScreenInfo * s);
void maximize(Client * c);
void get_mouse_position(Window w, int *x, int *y);

#endif /* JBWM_SCREEN_H */
