#ifndef JBWM_ATOM_H
#define JBWM_ATOM_H
#include "JBWMAtomName.h"
#include <X11/Xlib.h>
extern char * jbwm_atom_names[];
extern Atom jbwm_atoms[];
void jbwm_create_atoms(Display *d);
#endif//!JBWM_ATOM_H
