// Copyright 2016, Jeffrey E. Bedard

#ifndef JB_STRUCT_H
#define JB_STRUCT_H

#include <stdint.h>

struct JBSize16 {
	union {
		uint16_t width, w, columns, cols, col, c;
		int16_t top, t;
		int16_t max;
	};
	union {
		uint16_t height, h, rows, r;
		int16_t bottom, bot, b;
		int16_t min;
	};
};

struct JBSize8 {
	union {
		uint8_t width, w;
		uint8_t cols, c;
		int8_t ascent, a;
		int8_t top, t;
		int8_t max;
	};
	union {
		uint8_t height, h;
		uint8_t rows, r;
		int8_t descent, d;
		int8_t bottom, bot, b;
		int8_t min;
	};
};

#endif//!JB_STRUCT_H
