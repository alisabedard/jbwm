// Copyright 2016, Jeffrey E. Bedard
#ifndef JB_UTIL_H
#define JB_UTIL_H

#include <stdbool.h>

#undef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#undef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#undef SWAP
#define SWAP(type, a, b) {\
	type x; x = a; a = b; b = x;\
}

#undef likely
#define likely(x)       __builtin_expect((x), true)
#undef unlikely
#define unlikely(x)     __builtin_expect((x), false)

typedef int fd_t;

// Close the FD, print an error message if unsuccessful
void jb_close(const fd_t fd);

// If val is false, print msg, return val
bool jb_check(const bool val, const char * msg);

#endif//!JB_UTIL_H
