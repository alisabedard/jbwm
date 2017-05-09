// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#include "signal.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "config.h"
static void sigchld_cb(int sig __attribute__((unused)))
{
	wait(NULL);
}
static void jbwm_wait_child(void)
{
	signal(SIGCHLD, sigchld_cb); // reap zombies
}
void jbwm_start_terminal(void)
{
	if (fork() == 0) {
		char * t = getenv(JBWM_ENV_TERM);
		execlp(t, t, (char*)NULL);
		// This is only reached on error.
		exit(1);
	} else // in controlling process:
		jbwm_wait_child();
}
