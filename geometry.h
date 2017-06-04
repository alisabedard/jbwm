// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_GEOMETRY_H
#define JBWM_GEOMETRY_H
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_set_client_geometry(Display * d, struct JBWMClient * c);
#endif//!JBWM_GEOMETRY_H
