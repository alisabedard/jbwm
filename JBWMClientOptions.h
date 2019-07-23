// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef JBWM_JBWMCLIENTOPTIONS_H
#define JBWM_JBWMCLIENTOPTIONS_H
#include <stdbool.h>
#include <stdint.h>
struct JBWMClientOptions {
    uint8_t border : 2;
    bool fullscreen : 1;
    bool max_horz : 1;
    bool max_vert : 1;
    bool no_close : 1;
    bool no_max : 1;
    bool no_shade : 1;
    bool no_move : 1;
    bool no_resize : 1;
    bool no_title_bar : 1;
    bool remove : 1;
    bool shaded : 1;
    bool shaped : 1;
    bool sticky : 1;
    bool tearoff : 1;
};
#endif//!JBWM_JBWMCLIENTOPTIONS_H
