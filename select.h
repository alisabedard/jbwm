// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SELECT_H
#define JBWM_SELECT_H
struct JBWMClient;
// Set client as that receiving focus
void jbwm_select_client(struct JBWMClient * target,
    struct JBWMClient ** current_client);
#endif//!JBWM_SELECT_H
