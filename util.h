// Copyright 2017, Jeffrey E. Bedard
#ifndef LIBJB_UTIL_H
#define LIBJB_UTIL_H
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
#include <stdbool.h>
typedef int fd_t;
// If val is false, print msg then abort()
void jb_assert(const bool val, char * msg);
// If val is false, print msg then return !val
bool jb_check(const bool val, const char * msg);
// Execute command in a new background process
void jb_execvp(const char * command, char *const argv[]);
// If val is false, print msg then exit(1)
void jb_require(const bool val, const char * msg);
// Execute command in a new background shell
void jb_shell(const char * command);
/* Execute command in a new background process.  Note, this does not handle
   quoted arguments properly.  For that, use jb_shell().  This version has the
   advantage of not spawning a shell process.  */
void jb_system(const char * command);
#ifdef __cplusplus
}
#endif//__cplusplus
#endif//!LIBJB_UTIL_H
