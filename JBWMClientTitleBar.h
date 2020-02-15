// Copyright 2020, Jeffrey E. Bedard
#ifndef JBWM_JBWMCLIENTTITLEBAR_H
#define JBWM_JBWMCLIENTTITLEBAR_H
#include <X11/X.h>
struct JBWMClientTitleBar {
    Window win, close, resize, shade, stick;
};
#endif//!JBWM_JBWMCLIENTTITLEBAR_H
