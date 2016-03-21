// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_SCREEN_H
#define JBWM_SCREEN_H

void drag(Client * restrict c);
void moveresize(Client * restrict c);
void resize(Client * restrict c);
void hide(Client * restrict c);
void unhide(Client * restrict c);
uint8_t switch_vdesk(ScreenInfo * s, const uint8_t v);
void maximize_horz(Client * restrict c);
void maximize_vert(Client * restrict c);
void restore_horz(Client * restrict c);
void restore_vert(Client * restrict c);
void set_fullscreen(Client * restrict c);
void unset_fullscreen(Client * restrict c);
void set_maximized(Client * restrict c);
void unset_maximized(Client * restrict c);
void maximize(Client * restrict c);
// Used by vdesk switching and EWMH
#define DESKTOPS 10

#endif /* JBWM_SCREEN_H */
