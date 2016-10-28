// Copyright 2016, Jeffrey E. Bedard
#ifndef JBWM_MAX_H
#define JBWM_MAX_H
#include "JBWMClient.h"
void jbwm_set_horz(Display * restrict d, struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_set_not_horz(Display * restrict d, struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_set_vert(Display * restrict d, struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_set_not_vert(Display * restrict d, struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_set_fullscreen(Display * restrict d, struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_set_not_fullscreen(Display * restrict d,
	struct JBWMClient * restrict c) __attribute__((nonnull));
#endif//!JBWM_MAX_H
