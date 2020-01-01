// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_KEYS_H
#define JBWM_KEYS_H
#include <X11/Xlib.h>
void jbwm_grab_root_keys(Display * d, const Window root)
__attribute__((nonnull));
#endif//!JBWM_KEYS_H
