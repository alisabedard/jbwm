/*  jbwm - Minimalist Window Manager for X */
/*  Copyright 2008-2020, Jeffrey E. Bedard <jefbed@gmail.com> */
/*  Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk> */
/*  See README for license and other details. */
#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H
#include <stdint.h>
#include <X11/Xlib.h>
struct JBWMClient;
/*  Free client and destroy its windows and properties. */
void jbwm_client_free(struct JBWMClient * c,
    struct JBWMClient ** head_client);
/*  Get the client with input focus. */
//struct JBWMClient * jbwm_get_current_client(void);
void jbwm_hide_client(const struct JBWMClient * restrict c);
/*  Relink client linked list to exclude c */
void jbwm_relink_client_list(struct JBWMClient * c,
    struct JBWMClient ** head_client);
/*  Move the client to the specified virtual desktop */
void jbwm_set_client_vdesk(struct JBWMClient * restrict c,
    const uint8_t desktop);
void jbwm_toggle_sticky(struct JBWMClient * restrict c,
    struct JBWMClient ** current_client);
void jbwm_restore_client(const struct JBWMClient * restrict c);
__attribute__((pure))
/* Return the client that has specified window as either window or parent.
 *  This is the third most called function.  Show restraint in adding any
 *  future tests.  */
struct JBWMClient * jbwm_find_client(struct JBWMClient * restrict head,
    const Window w);
#endif/* !JBWM_CLIENT_H */
