// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_SNAP_H
#define JBWM_SNAP_H

#ifdef USE_SNAP

#include "JBWMClient.h"

void snap_border(struct JBWMClient * restrict c)
	__attribute__((nonnull));

void snap_client(struct JBWMClient * c)
	__attribute__((nonnull));

#else//!USE_SNAP

#define snap_border(c)
#define snap_client(c)

#endif//USE_SNAP

#endif//JBWM_SNAP_H
