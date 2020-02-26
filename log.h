// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_LOG_H
#define JBWM_LOG_H
#ifdef DEBUG
#include <stdio.h>
#define JBWM_LOG(...)\
{\
    fprintf(stderr, "%s:%d:%s: ", __FILE__, __LINE__,__FUNCTION__);\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\n");\
}
#else//!DEBUG
#define JBWM_LOG(...)
#endif//DEBUG
#endif//!JBWM_LOG_H
