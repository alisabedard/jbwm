#ifndef JBWM_DRAW_H
#define JBWM_DRAW_H
#include "JBWMScreen.h"
void jbwm_draw_string(struct JBWMScreen *s, Window const target,
  int16_t const x, int16_t const y, char *text, size_t const length);
#endif//!JBWM_DRAW_H
