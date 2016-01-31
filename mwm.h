// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com>

#ifndef MWM_H
#define MWM_H

#ifdef MWM
void handle_mwm_hints(Client * c);
#else
#define handle_mwm_hints(c)
#endif

#endif//!MWM_H
