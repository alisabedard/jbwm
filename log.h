// Copyright 2016, Jeffrey E. Bedard
#ifndef LIBJB_LOG_H
#define LIBJB_LOG_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define LIBJB_WARN(...) {\
	fprintf(stderr, __VA_ARGS__);\
	fputc('\n', stderr);\
}
#define LIBJB_ERROR(...) {\
	LIBJB_WARN(__VA_ARGS__);\
	exit(1);\
}
#ifdef DEBUG
#define LIBJB_LOG(...) {\
        fprintf(stderr, "%s:%d ", __FILE__, __LINE__);\
        LIBJB_WARN(__VA_ARGS__);\
}
#else//!DEBUG
#define LIBJB_LOG(...)
#endif//DEBUG
#ifndef LOG// Avoid namespace clashes, defer to environment
#define LOG LIBJB_LOG
#endif//!LOG
// Used for simplistic profiling and code path testing:
#define LIBJB_VCOUNT(v) {\
	static uint32_t v; ++v; printf(#v ": %d\n", v);\
}
#endif//!LIBJB_LOG_H
