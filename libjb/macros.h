// Copyright 2017, Jeffrey E. Bedard
#ifndef LIBJB_MACROS_H
#define LIBJB_MACROS_H
#define JB_MAX(a,b) ((a) > (b) ? (a) : (b))
#define JB_MIN(a,b) ((a) < (b) ? (a) : (b))
#define JB_LIMIT(var, max, min) var = JB_MAX(JB_MIN(var, max), min);
#define JB_SWAP(type, a, b) {type c = a; a = b; b = c;}
#define JB_LIKELY(x)       __builtin_expect((x), true)
#define JB_UNLIKELY(x)     __builtin_expect((x), false)
#endif//!LIBJB_MACROS_H
