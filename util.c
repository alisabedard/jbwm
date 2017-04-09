// Copyright 2017, Jeffrey E. Bedard
#include "util.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
static void wait_cb(int sig __attribute((unused)))
{
	wait(NULL);
}
// Execute command in a new background process
void jb_execvp(const char * command, char *const argv[])
{
	if (fork() == 0) { // child:
		errno = 0;
		execvp(command, argv);
		// execlp only returns on error
		exit(1);
	} else { // in controlling process:
		static bool wait_cb_registered;
		if (!wait_cb_registered) {
			signal(SIGCHLD, wait_cb);
			wait_cb_registered = true;
		}
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
