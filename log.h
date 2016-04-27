// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM__LOG_H__
#define JBWM__LOG_H__

#ifdef DEBUG

#include <stdio.h>

#define LOG(...) \
	{\
		fprintf(stderr, "%s:%d:  ", __FILE__, __LINE__);\
		fprintf(stderr, __VA_ARGS__);\
		putc('\n', stderr);\
	}

#else//!DEBUG

#define LOG(...)
#define MARK()

#endif//DEBUG

#endif /* JBWM__LOG_H__ */
