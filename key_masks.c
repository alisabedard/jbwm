// Copyright 2020, Jeffrey E. Bedard
#include "key_masks.h"
#include "config.h"
static uint16_t grab_mask = JBWM_KEYMASK_GRAB;
                //mod_mask = JBWM_KEYMASK_MOD;
void jbwm_set_grab_mask(const uint16_t mask)
{
  grab_mask = mask;
}
//void jbwm_set_mod_mask(const uint16_t mask)
//{
//  mod_mask = mask;
//}
uint16_t jbwm_get_grab_mask(void)
{
  return grab_mask;
}
//uint16_t jbwm_get_mod_mask(void)
//{
////  return mod_mask;
//  return JBWM_KEYMASK_MOD
//}
