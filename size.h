// Copyright 2016, Jeffrey E. Bedard

#ifndef JB_SIZE_H
#define JB_SIZE_H

#include <stdint.h>

struct JBDim {
	union {
		uint16_t width, w, columns, cols, col, c;
		int16_t x, top, t, max;
	};
	union {
		uint16_t height, h, row, rows, r;
		int16_t y, bottom, bot, b, min;
	};
};

#endif//!JB_SIZE_H
