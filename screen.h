/* Copyright 2007-2011, Jeffrey E. Bedard */
#ifndef JBWM_SCREEN_H
#define JBWM_SCREEN_H

typedef struct
{
	short x, y;
} Position;

void drag(Client * c);
void moveresize(Client * c);
void show_info(Client * c, KeySym key);
void sweep(Client * c);
void next(void);
void hide(Client * c);
void unhide(Client * c);
void switch_vdesk(ScreenInfo * s, const ubyte v);
void grab_keys_for_screen(ScreenInfo * s);
void maximize(Client * c);

#define get_mouse_position(x, y, root) XQueryPointer(jbwm.X.dpy, \
		root, &root, &root, x, y, (int *)&root, (int *)&root, \
		(unsigned int *)&root);

#define setmouse(w, x, y) XWarpPointer(jbwm.X.dpy, None, w, 0, 0, 0, 0, x, y)

#endif /* JBWM_SCREEN_H */
