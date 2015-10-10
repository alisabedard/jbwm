/*  JBWM Copyright 2008, Jeffrey E. Bedard <antiright@gmail.com> */

#ifndef JBWM_NEW_H
#define JBWM_NEW_H

void get_wm_normal_hints(Client * c);

void *jbwm_get_property(Window w, Atom property, Atom req_type,
	unsigned long *nitems_return);

#ifdef DEBUG
void print_size_hints(Window w);
#endif /* DEBUG */

#endif /* JBWM_NEW_H */
