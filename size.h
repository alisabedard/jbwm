// Copyright 2016, Jeffrey E. Bedard

#ifndef JB_STRUCT_H
#define JB_STRUCT_H

#include <stdint.h>

struct JBSize {
	union {
		uint16_t width, w, columns, cols, col, c;
		int16_t top, t;
	};
	union {
		uint16_t height, h, rows, r;
		int16_t bottom, bot, b;
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
