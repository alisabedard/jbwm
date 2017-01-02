// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_JBWM_H
#define JBWM_JBWM_H
#include <X11/Xlib.h>
#include <stdint.h>
__attribute__((noreturn))
void jbwm_error(const char * msg);
struct JBWMScreen * jbwm_get_screens(void);
void jbwm_init_screen(Display * d, const uint8_t i);
void jbwm_parse_command_line(const uint8_t argc, char **argv);
void jbwm_set_defaults(void);
void jbwm_set_screens(struct JBWMScreen * restrict s);
#endif /* not JBWM_JBWM_H */
