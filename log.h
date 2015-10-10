/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#ifndef ARWM__LOG_H__
#define ARWM__LOG_H__

#include <stdio.h>

#define LOG_INFO(...) printf(__VA_ARGS__);
#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__);

#ifdef DEBUG
#define LOG_DEBUG(...) \
{\
	fprintf(stderr, "%s:%d:  ", __FILE__, __LINE__);\
	fprintf(stderr, __VA_ARGS__);\
}
#define MARK fprintf(stderr, "%s:  %d\n", __FILE__, __LINE__);
#else
#define LOG_DEBUG(...)
#define MARK
#endif

#ifdef XDEBUG
#define LOG_XDEBUG(...) fprintf(stderr, __VA_ARGS__);
#else
#define LOG_XDEBUG(...)
#endif

#endif /* ARWM__LOG_H__ */
