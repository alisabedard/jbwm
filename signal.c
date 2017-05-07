// Copyright 2017, Jeffrey E. Bedard <jefbed@gmail.com>
#include "signal.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
static void sigchld_cb(int sig __attribute__((unused)))
{
	wait(NULL);
}
void jbwm_wait_child(void)
{
	signal(SIGCHLD, sigchld_cb); // reap zombies
}
