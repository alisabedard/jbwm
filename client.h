// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H
#include <stdint.h>
#include <X11/Xlib.h>
struct JBWMClient;
// Search for the JBWMClient matching the specified window
struct JBWMClient * jbwm_get_client(Window w)
	__attribute__((pure));
// Get the client with input focus.
struct JBWMClient * jbwm_get_current_client(void);
// Get the head of the JBWMClient linked list
struct JBWMClient * jbwm_get_head_client(void);
/* Set c as the new head client, linking its next element
   to the previous head */
void jbwm_prepend_client(struct JBWMClient * restrict c);
// Relink c's linked list to exclude c
void jbwm_relink_client_list(struct JBWMClient * restrict c);
// Move the client to the specified virtual desktop
void jbwm_set_client_vdesk(struct JBWMClient * restrict c, const uint8_t d)
	__attribute((nonnull));
// Assign c as the current client
void jbwm_set_current_client(struct JBWMClient * restrict c);
void jbwm_toggle_sticky(struct JBWMClient * restrict c) __attribute__((nonnull));
#endif//!JBWM_CLIENT_H
