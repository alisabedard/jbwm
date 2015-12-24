// Copyright 2015, Jeffrey E. Bedard
// Convenience macros for dealing with X properties.  

#ifndef ATOMS_H
#define ATOMS_H

#define XA(atom) XInternAtom(D, atom, false)
#define XPROP(win, prop, type, data, size) XChangeProperty(D, win, \
	prop, type, 32, PropModeReplace, (const unsigned char *)(data), size)

#endif /* ATOMS_H */
