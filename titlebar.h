// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_TITLEBAR_H
#define JBWM_TITLEBAR_H

void 
update_titlebar(Client * c);

/* Ensure that the title text fits within the titlebar.  */
#ifdef USE_TBAR
#define TDIM (jbwm.X.font->ascent+jbwm.X.font->descent)
#else /* !USE_TBAR */
#define TDIM 0
#endif

#endif /* JBWM_TITLEBAR_H */
