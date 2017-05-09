// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#include "exec.h"
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
static void jbwm_exec(const char * command)
{
	if (fork() == 0) {
		execl("/bin/sh", "sh", "-c", command, NULL);
		// This is only reached on error.
		exit(1);
	} else // in controlling process:
		jbwm_wait_child();
}
void jbwm_start_terminal(void)
{
	jbwm_exec(getenv(JBWM_ENV_TERM));
}
