// Copyright 2016, Jeffrey E. Bedard
#ifndef JB_UTIL_H
#define JB_UTIL_H
#include <stdbool.h>
#define JB_MAX(a,b) ((a) > (b) ? (a) : (b))
#define JB_MIN(a,b) ((a) < (b) ? (a) : (b))
#define JB_LIMIT(var, max, min) {var = JB_MAX(JB_MIN(var, max), min);}
#define JB_SWAP(type, a, b) {type c = a; a = b; b = c;}
#define JB_LIKELY(x)       __builtin_expect((x), true)
#define JB_UNLIKELY(x)     __builtin_expect((x), false)
typedef int fd_t;
// If val is false, print msg then return !val
bool jb_check(const bool val, const char * msg);
// If val is false, print msg then exit(1)
void jb_require(const bool val, const char * msg);
// If val is false, print msg then abort()
void jb_assert(const bool val, char * msg);
#endif//!JB_UTIL_H
