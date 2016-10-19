// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SNAP_H
#define JBWM_SNAP_H
#ifdef JBWM_USE_SNAP
#include "JBWMClient.h"
void jbwm_snap_border(struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_snap_client(struct JBWMClient * c)
	__attribute__((nonnull));
#else//!JBWM_USE_SNAP
#define jbwm_snap_border(c)
#define jbwm_snap_client(c)
#endif//JBWM_USE_SNAP
#endif//JBWM_SNAP_H
