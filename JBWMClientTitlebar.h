// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_JBWMCLIENTTITLEBAR_H
#define JBWM_JBWMCLIENTTITLEBAR_H
#ifdef JBWM_USE_TITLE_BAR
#include <X11/X.h>
struct JBWMClientTitlebar {
	Window win, close, resize, shade, stick;
};
#endif//JBWM_USE_TITLE_BAR
#endif//!JBWM_JBWMCLIENTTITLEBAR_H

