// Copyright 2016, Jeffrey E. Bedard

#ifndef JBWM_MAX_H
#define JBWM_MAX_H

#include "JBWMClient.h"

void set_horz(struct JBWMClient * restrict c)
	__attribute__((nonnull));

void unset_horz(struct JBWMClient * restrict c)
	__attribute__((nonnull));

void set_vert(struct JBWMClient * restrict c)
	__attribute__((nonnull));

void unset_vert(struct JBWMClient * restrict c)
	__attribute__((nonnull));

void set_fullscreen(struct JBWMClient * restrict c)
	__attribute__((nonnull));

void unset_fullscreen(struct JBWMClient * restrict c)
	__attribute__((nonnull));

#endif//!JBWM_MAX_H
