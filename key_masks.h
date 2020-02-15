// Copyright 2020, Jeffrey E. Bedard
#ifndef JBWM_KEY_MASKS_H
#define JBWM_KEY_MASKS_H
#include <stdint.h>
uint16_t jbwm_get_grab_mask(void);
uint16_t jbwm_get_mod_mask(void);
void jbwm_set_grab_mask(const uint16_t mask);
void jbwm_set_mod_mask(const uint16_t mask);
#endif//!JBWM_KEY_MASKS_H
