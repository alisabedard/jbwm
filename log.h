// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_LOG_H
#define JBWM_LOG_H
#ifdef DEBUG
#include <stdio.h>
#define JBWM_LOG(...)\
{\
	fprintf(stderr, __VA_ARGS__);\
	fprintf(stderr, " @ %s:%d\n", __FILE__, __LINE__);\
}
#else//!DEBUG
#define JBWM_LOG(...)
#endif//DEBUG
#endif//!JBWM_LOG_H
