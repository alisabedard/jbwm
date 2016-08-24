// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com> 

#ifndef JB_STRUCT_H
#define JB_STRUCT_H

#include <stdint.h>

struct JBSize {
	union {
		uint16_t width, w, top, t;
	};
	union {
		uint16_t height, h, bottom, b;
	};
};

struct JBCharSize {
	union {
		uint8_t width, w;
	};
	union {
		uint8_t height, h;
	};
};


#endif//!JB_STRUCT_H
