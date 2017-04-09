// Copyright 2017, Jeffrey E. Bedard
#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
static void sigchld(int sig __attribute((unused)))
{
	wait(NULL);
}
// Execute command in a new background process
void jb_system(const char * command)
{
	if (fork() == 0) { // child:
		execlp(command, command, (char *)NULL);
		// execlp only returns on error
	} else { // in controlling process:
		signal(SIGCHLD, sigchld);
	}
}
// If val is false, print msg then return !val
bool jb_check(const bool val, const char * msg)
{
	if (!val) { // failure
		uint16_t l = 0;
		while (msg[++l]);
		write(2, msg, l);
		write(2, "\n", 1);
	}
	return !val;
}
// If val is false, print msg then exit(1)
void jb_require(const bool val, const char * msg)
{
	if (jb_check(val, msg))
		exit(1);
}
// If val is false, print msg then abort()
void jb_assert(const bool val, char * msg)
{
	if (jb_check(val, msg))
		abort();
}
