// Copyright 2020, Alisa Bedard <alisabedard@gmail.com>
#ifndef JBWM_JBWMPOINT_H
#define JBWM_JBWMPOINT_H
#include <stdint.h>
union JBWMPoint {
  struct{
    int16_t x, y;
  };
  int16_t array[2];
};
#endif//!JBWM_JBWMPOINT_H
