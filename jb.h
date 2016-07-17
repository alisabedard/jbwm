// Copyright 2016, Jeffrey E. Bedard

#ifndef JB_H
#define JB_H

#undef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#undef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#undef SWAP
#define SWAP(type, a, b) { type x; x = a; a = b; b = x;

#endif//!JB_H
