/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#ifndef JBWM__LOG_H__
#define JBWM__LOG_H__

#include <stdio.h>

#define LOG_INFO(...) printf(__VA_JBGS__);
#define LOG_ERROR(...) fprintf(stderr, __VA_JBGS__);

#ifdef DEBUG
#define LOG_DEBUG(...) \
{\
	fprintf(stderr, "%s:%d:  ", __FILE__, __LINE__);\
	fprintf(stderr, __VA_JBGS__);\
}
#define MJBK fprintf(stderr, "%s:  %d\n", __FILE__, __LINE__);
#else
#define LOG_DEBUG(...)
#define MJBK
#endif

#ifdef XDEBUG
#define LOG_XDEBUG(...) fprintf(stderr, __VA_JBGS__);
#else
#define LOG_XDEBUG(...)
#endif

#endif /* JBWM__LOG_H__ */
