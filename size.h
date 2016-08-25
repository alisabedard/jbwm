// Copyright 2016, Jeffrey E. Bedard

#ifndef JB_STRUCT_H
#define JB_STRUCT_H

#include <stdint.h>

struct JBSize {
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

struct JBPoint {
	union {
		int16_t x, column, col, c, top, t;
	};
	union {
		int16_t y, row, r, bot, b;
	};
};

union JBDim {
	struct JBPoint p;
	struct JBSize s;
};

#endif//!JB_STRUCT_H
