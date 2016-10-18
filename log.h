// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM__LOG_H__
#define JBWM__LOG_H__
#ifdef DEBUG
#include <stdio.h>
#define LOG(...)\
{\
	fprintf(stderr, __VA_ARGS__);\
	fprintf(stderr, " @ %s:%d\n", __FILE__, __LINE__);\
}
#else//!DEBUG
#define LOG(...)
#endif//DEBUG
#endif /* JBWM__LOG_H__ */
