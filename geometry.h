// Copyright 2020, Jeffrey E. Bedard
#ifndef JBWM_GEOMETRY_H
#define JBWM_GEOMETRY_H
#include <stdint.h>
#include <X11/Xlib.h>
#include "JBWMSize.h"
struct JBWMClient;
//struct JBWMSize jbwm_get_display_size(Display * d,
//    const uint8_t screen) __attribute__((pure));
void jbwm_set_client_geometry(struct JBWMClient * c);
#endif//!JBWM_GEOMETRY_H
