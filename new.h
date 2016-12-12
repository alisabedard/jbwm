// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_NEW_H
#define JBWM_NEW_H
#include "typedef.h"
struct JBWMScreen;
void jbwm_new_client(struct JBWMScreen * s, const jbwm_window_t w);
#endif//!JBWM_NEW_H
