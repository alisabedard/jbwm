// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_JBWM_H
#define JBWM_JBWM_H
#include <X11/Xlib.h>
struct JBWMClient;
struct JBWMScreen;
void jbwm_init_screens(Display *d, struct JBWMScreen *s,
  struct JBWMClient ** head_client, struct JBWMClient ** current_client,
  const short screens);
#endif /* not JBWM_JBWM_H */
