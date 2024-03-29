// Copyright 2020, Alisa Bedard
#ifndef JBWM_MAX_H
#define JBWM_MAX_H
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_set_horz(struct JBWMClient * c)
__attribute__((nonnull));
void jbwm_set_not_horz(struct JBWMClient * c)
__attribute__((nonnull));
void jbwm_set_vert(struct JBWMClient * c)
__attribute__((nonnull));
void jbwm_set_not_vert(struct JBWMClient * c)
__attribute__((nonnull));
void jbwm_set_fullscreen(struct JBWMClient * c)
__attribute__((nonnull));
void jbwm_set_not_fullscreen(struct JBWMClient * c)
__attribute__((nonnull));
#endif//!JBWM_MAX_H
