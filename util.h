// Copyright 2016, Jeffrey E. Bedard
#ifndef LIBJB_UTIL_H
#define LIBJB_UTIL_H
#include <stdbool.h>
typedef int fd_t;
// If val is false, print msg then return !val
bool jb_check(const bool val, const char * msg);
// If val is false, print msg then exit(1)
void jb_require(const bool val, const char * msg);
// If val is false, print msg then abort()
void jb_assert(const bool val, char * msg);
#endif//!LIBJB_UTIL_H
