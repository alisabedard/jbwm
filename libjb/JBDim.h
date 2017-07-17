// Copyright 2017, Jeffrey E. Bedard
#ifndef LIBJB_JBDIM_H
#define LIBJB_JBDIM_H
#include <stdint.h>
struct JBDim {
	/* Note:  Everything except width and height, and their aliases
	   w and h, should be kept as int16_t.  */
	union {
		uint16_t width, w;
		int16_t columns, cols, col, c;
		int16_t maximum, max;
		int16_t start;
		int16_t top, t;
		int16_t x;
	};
	union {
		uint16_t height, h;
		int16_t bottom, bot, b;
		int16_t end;
		int16_t index;
		int16_t length;
		int16_t minimum, min;
		int16_t row, rows, r;
		int16_t y;
	};
};
#endif//!LIBJB_JBDIM_H
