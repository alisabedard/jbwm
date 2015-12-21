/*
 * jbwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#ifndef JBWM__LOG_H__
#define JBWM__LOG_H__

#ifdef STDIO
#include <stdio.h>
#define ERROR(...) {fprintf(stderr, __VA_ARGS__); putc('\n', stderr); \
	exit(1);}
#else//!STDIO
#define ERROR(...) exit(1)
#endif//STDIO

#if defined(DEBUG) && defined(STDIO)
#define LOG(...) \
{\
	fprintf(stderr, "%s:%d:  ", __FILE__, __LINE__);\
	fprintf(stderr, __VA_ARGS__); putc('\n', stderr);\
}
#define MARK() LOG("MARK")
#else // !DEBUG || !STDIO 
#define LOG(...)
#define MARK()
#endif // DEBUG && STDIO

#define UNUSED(x)

#endif /* JBWM__LOG_H__ */
