// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef JBWM_DRAG_H
#define JBWM_DRAG_H
#include <stdbool.h>
struct JBWMClient;
/* Drag the specified client.  Resize the client if resize is true.  */
void jbwm_drag(struct JBWMClient * restrict c, const bool resize)
	__attribute__((nonnull));
#endif//!JBWM_DRAG_H
