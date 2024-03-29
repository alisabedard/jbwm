// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SHAPE_H
#define JBWM_SHAPE_H
struct JBWMClient;
void jbwm_new_shaped_client(struct JBWMClient * c);
void jbwm_set_shape(struct JBWMClient * c);
#endif//!JBWM_SHAPE_H
