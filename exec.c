// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#include "exec.h"
#include <signal.h> // for signal()
#include <stdlib.h> // for exit()
#include <unistd.h> // for fork()
#include "config.h" // for JBWM_ENV_TERM
static void jbwm_exec(const char * command)
{
	if (fork() == 0) {
		execl("/bin/sh", "sh", "-c", command, NULL);
		// This is only reached on error.
		exit(1);
	} else // in controlling process:
		signal(SIGCHLD, SIG_IGN);
}
void jbwm_start_terminal(void)
{
	jbwm_exec(getenv(JBWM_ENV_TERM));
}
