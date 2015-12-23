#ifndef ATOMS_H
#define ATOMS_H

#define XA(atom) XInternAtom(D, atom, false)

#define JSETPROP(win, prop, type, data, sz) XChangeProperty(D, win, \
	XA(prop), XA_CARDINAL, 32, PropModeReplace, \
	(const unsigned char *)data, sz)
#define JCARD(win, prop, data, sz) JSETPROP(win, prop, XA_CARDINAL, data, sz)
#define JSTRING(win, prop, data, sz) JSETPROP(win, prop, XA_STRING, data, sz)
#define JWINDOW(win, prop, data, sz) JSETPROP(win, prop, XA_WINDOW, data, sz)
#define JATOM(win, prop, data, sz) JSETPROP(win, prop, XA_ATOM, data, sz)

#endif /* ATOMS_H */
