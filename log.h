// Copyright 2016, Jeffrey E. Bedard
#ifndef JB_LOG_H
#define JB_LOG_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define WARN(...) {\
	fprintf(stderr, __VA_ARGS__);\
	fputc('\n', stderr);\
}
#define ERROR(...) {\
	WARN(__VA_ARGS__);\
	exit(1);\
}
#ifdef DEBUG
#define LOG(...) {\
        fprintf(stderr, "%s:%d ", __FILE__, __LINE__);\
        WARN(__VA_ARGS__);\
}
#else//!DEBUG
#define LOG(...)
#endif//DEBUG
// Used for simplistic profiling and code path testing:
#define VCOUNT(v) {\
	static uint32_t v; ++v; printf(#v ": %d\n", v);\
}
#endif//!JB_LOG_H
