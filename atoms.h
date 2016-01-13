// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

// Convenience macros for dealing with X properties.  

#ifndef ATOMS_H
#define ATOMS_H

#define XA(atom) XInternAtom(D, atom, false)
#define XPROP(win, prop, type, data, size) XChangeProperty(D, win, \
	prop, type, 32, PropModeReplace, (const unsigned char *)(data), size)

#endif /* ATOMS_H */
