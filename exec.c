// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#include "exec.h"
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
void jbwm_exec(const char * command)
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
