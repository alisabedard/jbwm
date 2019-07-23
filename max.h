// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_MAX_H
#define JBWM_MAX_H
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_set_horz(struct JBWMClient * restrict c)
__attribute__((nonnull));
void jbwm_set_not_horz(struct JBWMClient * restrict c)
__attribute__((nonnull));
void jbwm_set_vert(struct JBWMClient * restrict c)
__attribute__((nonnull));
void jbwm_set_not_vert(struct JBWMClient * restrict c)
__attribute__((nonnull));
void jbwm_set_fullscreen(struct JBWMClient * restrict c)
__attribute__((nonnull));
void jbwm_set_not_fullscreen(struct JBWMClient * restrict c)
__attribute__((nonnull));
#endif//!JBWM_MAX_H
