// Copyright 2016, Jeffrey E. Bedard
#ifndef JB_UTIL_H
#define JB_UTIL_H

#include <stdbool.h>

#define JB_MAX(a,b) ((a) > (b) ? (a) : (b))
#define JB_MIN(a,b) ((a) < (b) ? (a) : (b))
#define JB_LIMIT(var, max, min) {var = MAX(MIN(var, max), min);}
#define JB_SWAP(type, a, b) {type c = a; a = b; b = c;}
#define JB_LIKELY(x)       __builtin_expect((x), true)
#define JB_UNLIKELY(x)     __builtin_expect((x), false)

typedef int fd_t;

// Close the FD, print an error message if unsuccessful
void jb_close(const fd_t fd);

// Open path with flags options, check result
fd_t jb_open(const char * path, const int flags);

// If val is false, print msg and abort
bool jb_abort_if_false(bool val, char * msg);

// If val is false, print msg, using perror if errno set, return !val
bool jb_check(const bool val, char * msg);

// If val is false, print msg and exit
void jb_assert(const bool val, char * msg);

#endif//!JB_UTIL_H
