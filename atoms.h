#ifndef ATOMS_H
#define ATOMS_H

#define GETATOM(atom) XInternAtom(jbwm.X.dpy, atom, false)
#define JA(atom) GETATOM(atom)

#define JA_WM_STATE JA("WM_STATE")
#define JA_WM_PROTOS JA("WM_PROTOCOLS")
#define JA_WM_DELETE JA("WM_DELETE_WINDOW")

#define JA_VWM_STATE JA("_NET_WM_STATE")
#define JA_VWM_DESKTOP JA("_NET_WM_DESKTOP")
#define JA_VWM_STICKY JA("_NET_WM_STICKY")

#define JA_MWM_HINTS JA("_XA_MWM_HINTS")

#endif /* ATOMS_H */
