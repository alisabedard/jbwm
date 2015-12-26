/*  JBWM Copyright 2008, Jeffrey E. Bedard <jefbed@gmail.com> */

#ifndef JBWM_NEW_H
#define JBWM_NEW_H

void *
get_property(Window w, Atom property, Atom type, unsigned long *num_items);

void make_new_client(Window w, ScreenInfo *s);

#endif /* JBWM_NEW_H */
