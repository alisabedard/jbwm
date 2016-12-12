// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H
#include "JBWMClient.h"
struct JBWMClientManager;
// Search for the JBWMClient matching the specified window
struct JBWMClient * jbwm_get_client(jbwm_window_t w)
	__attribute__((hot,pure));
struct JBWMClientManager * jbwm_get_client_manager(void);
// Get the client with input focus.
struct JBWMClient * jbwm_get_current_client(void);
// Get the head of the JBWMClient linked list
struct JBWMClient * jbwm_get_head_client(void);
// Relink c's linked list to exclude c
void jbwm_relink_client_list(struct JBWMClient * restrict c);
// Move the client to the specified virtual desktop
void jbwm_set_client_vdesk(struct JBWMClient * restrict c, const uint8_t d)
	__attribute((nonnull));
// Assign c to the head of the JBWMClient linked list.
void jbwm_set_head_client(struct JBWMClient * restrict c);
void jbwm_toggle_sticky(struct JBWMClient * restrict c) __attribute__((nonnull));
#endif//!JBWM_CLIENT_H
