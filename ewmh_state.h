#ifndef JBWM_EWMH_STATE_H
#define JBWM_EWMH_STATE_H
#ifdef JBWM_EWMH
#include "JBWMClient.h"
void ewmh_remove_state(const Window w, const Atom state);
void ewmh_add_state(const Window w, Atom state);
void ewmh_client_message(XClientMessageEvent * restrict e,
	struct JBWMClient * restrict c)
	__attribute__((nonnull(1)));
#else//!JBWM_EWMH
#define ewmh_remove_state(w, s)
#define ewmh_add_state(w, s)
#define ewmh_client_message(e, c)
#endif//JBWM_EWMH
#endif//JBWM_EWMH_STATE_H
