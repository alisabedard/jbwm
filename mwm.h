// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_MWM_H
#define JBWM_MWM_H
#ifdef JBWM_USE_MWM
#include "JBWMClient.h"
void jbwm_handle_mwm_hints(struct JBWMClient * c);
#else//!JBWM_USE_MWM
#define jbwm_handle_mwm_hints(c)
#endif//JBWM_USE_MWM
#endif//!JBWM_MWM_H
