// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef SNAP_H
#define SNAP_H
#ifdef USE_SNAP
void snap_border(Client * c);
void snap_client(Client * c);
#else
#define snap_border(c)
#define snap_client(c)
#endif
#endif//SNAP_H
