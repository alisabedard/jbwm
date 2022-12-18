// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_TITLE_BAR_H
#define JBWM_TITLE_BAR_H
struct JBWMClient;
void jbwm_toggle_shade(struct JBWMClient * c);
void jbwm_update_title_bar(struct JBWMClient * c);
#endif /* JBWM_TITLE_BAR_H */
