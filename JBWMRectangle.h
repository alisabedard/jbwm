// Copyright 2020, Alisa Bedard <alisabedard@gmail.com>
#ifndef JBWM_JBWMRECTANGLE_H
#define JBWM_JBWMRECTANGLE_H
#include <stdint.h>
union JBWMRectangle {
  struct {
    int16_t x, y, width, height;
  };
  int16_t array[4];
};
#endif//!JBWM_JBWMRECTANGLE_H
