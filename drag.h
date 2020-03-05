// Copyright 2020, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef JBWM_DRAG_H
#define JBWM_DRAG_H
#include <X11/Xlib.h>
#include <stdbool.h>
struct JBWMClient;
/* Drag the specified client.  Resize the client if resize is true.  */
void jbwm_drag(struct JBWMClient * c, struct JBWMClient ** head_client,
    const bool resize)
__attribute__((nonnull));
/* Warp the pointer to the specified position.  */
inline void jbwm_warp(Display * dpy, const Window w, const short x,
    const short y)
{
    XWarpPointer(dpy, None, w, 0, 0, 0, 0, x, y);
}
#endif//!JBWM_DRAG_H
