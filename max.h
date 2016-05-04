#ifndef JBWM_MAX_H
#define JBWM_MAX_H

#include "client_t.h"

void set_horz(Client * restrict c) __attribute__((nonnull));
void unset_horz(Client * restrict c) __attribute__((nonnull));

void set_vert(Client * restrict c) __attribute__((nonnull));
void unset_vert(Client * restrict c) __attribute__((nonnull));

void set_fullscreen(Client * restrict c) __attribute__((nonnull));
void unset_fullscreen(Client * restrict c) __attribute__((nonnull));

#endif//!JBWM_MAX_H
