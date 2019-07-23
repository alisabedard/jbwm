// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#ifndef JBWM_EXEC_H
#define JBWM_EXEC_H
#include <stdlib.h>
#include "config.h"
void jbwm_exec(const char * command);
inline void jbwm_start_terminal(void)
{
    jbwm_exec(getenv(JBWM_ENV_TERM));
}
#endif//!JBWM_EXEC_H
