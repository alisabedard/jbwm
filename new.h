// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_NEW_H
#define JBWM_NEW_H

void *
get_property(Window w, Atom property, Atom type, unsigned long *num_items);

void make_new_client(Window w, ScreenInfo *s);

#endif /* JBWM_NEW_H */
