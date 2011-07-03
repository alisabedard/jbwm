/* Copyright 2008, Jeffrey E. Bedard <antiright@gmail.com> */

#ifndef MACROS
#define MACROS

#include "flags.h"

#ifndef MIN
#define MIN(x, y) (((x)>(y)) ? (x) : (y))
#endif /* ! MIN */

#ifndef MAX
#define MAX(x, y) (((x)>(y)) ? (x) : (y))
#endif /* ! MAX */

/* Simple object oriented programming interface for C.  */

#define AR_STACK_METHOD(class, method, ...)\
	(class.method(&class, ##__VA_ARGS__))

#define ARMETHOD(class, method, ...)\
	(class->method(class, ##__VA_ARGS__))

#define $(...) ARMETHOD(__VA_ARGS__)

#define ARNEW(package, class, ...)\
	(package##_new_##class(__VA_ARGS__))

#define ARDELETE(class)\
	{ if(class) { $(class, delete); } }

#define $$(...) $(this, __VA_ARGS__)

#define ARINHERIT(parent, child, member)\
	child->member=parent->member

#define AROVERRIDE(parent, child, member)\
	parent->member=child->member

#endif /* MACROS */

