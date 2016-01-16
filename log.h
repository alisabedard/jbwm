// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM__LOG_H__
#define JBWM__LOG_H__

#ifdef STDIO
#include <stdio.h>
#define ERROR(msg) {fprintf(stderr, "%s\n", msg); exit(1);}
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

#ifdef XDEBUG
#define XLOG(...) LOG(__VA_ARGS__)
#else//!XDEBUG
#define XLOG(...)
#endif//XDEBUG

#define UNUSED(x)

#endif /* JBWM__LOG_H__ */
