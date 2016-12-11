#ifndef JBWM_EWMH_STATE_H
#define JBWM_EWMH_STATE_H
#ifdef JBWM_USE_EWMH
#include "JBWMClient.h"
void jbwm_ewmh_remove_state(Display * d,
	const jbwm_window_t w, const jbwm_window_t state);
void jbwm_ewmh_add_state(Display * d, const jbwm_window_t w,
	jbwm_atom_t state);
void jbwm_ewmh_handle_client_message(XClientMessageEvent * e,
	struct JBWMClient * restrict c)
	__attribute__((nonnull(1)));
#else//!JBWM_USE_EWMH
#define jbwm_ewmh_remove_state(d, w, s)
#define jbwm_ewmh_add_state(d, w, s)
#define jbwm_ewmh_handle_client_message(e, c)
#endif//JBWM_USE_EWMH
#endif//JBWM_EWMH_STATE_H
