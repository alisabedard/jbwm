/* Copyright 2008, Jeffrey E. Bedard <antiright@gmail.com> */

#ifndef MACROS
#define MACROS

#include "flags.h"

#ifndef MAX
#define MAX(x, y) (((x)>(y)) ? (x) : (y))
#endif /* ! MAX */

#include <stdio.h>

#ifdef DEBUG
#define LOG(msg) fprintf(stderr, "%s:%d - %s\n", __FILE__, __LINE__, msg)
#else /* !DEBUG */
#define LOG(msg)
#endif /* DEBUG */

#define LOG_ERROR(msg) {fputs(msg, stderr); exit(1);}

#endif /* MACROS */
