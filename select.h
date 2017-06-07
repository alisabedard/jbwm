// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SELECT_H
#define JBWM_SELECT_H
#include <X11/Xlib.h>
struct JBWMClient;
// Set client as that receiving focus
void jbwm_select_client(struct JBWMClient * c);
#endif//!JBWM_SELECT_H
