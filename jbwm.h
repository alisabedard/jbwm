// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_JBWM_H
#define JBWM_JBWM_H
__attribute__((noreturn))
void jbwm_error(const char * msg);
struct JBWMScreen * jbwm_get_screens(void);
#endif /* not JBWM_JBWM_H */
