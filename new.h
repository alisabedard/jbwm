/*  ARWM Copyright 2008, Jeffrey E. Bedard <antiright@gmail.com> */

#ifndef ARWM_NEW_H
#define ARWM_NEW_H

void *arwm_get_property(Window w, Atom property, Atom req_type,
	unsigned long *nitems_return);

#ifdef DEBUG
void print_size_hints(Window w);
#endif /* DEBUG */

#endif /* ARWM_NEW_H */
