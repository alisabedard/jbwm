/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#ifndef JBWM__LOG_H__
#define JBWM__LOG_H__

#include <stdio.h>

#define ERROR(...) {fprintf(stderr, __VA_ARGS__); exit(1);}

#ifdef DEBUG
#define LOG(...) \
{\
	fprintf(stderr, "%s:%d:  ", __FILE__, __LINE__);\
	fprintf(stderr, __VA_ARGS__); putc('\n', stderr);\
}
#define MARK() LOG("MARK")
#else
#define LOG(...)
#define MARK()
#endif

#define UNUSED(x)

#endif /* JBWM__LOG_H__ */
