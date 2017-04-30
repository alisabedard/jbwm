// Copyright 2017, Jeffrey E. Bedard
#include "util.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h> // for FreeBSD
#include <sys/wait.h>
#include <unistd.h>
static void wait_cb(int sig __attribute((unused)))
{
	wait(NULL);
}
static void init_wait_cb(void)
{
	static bool wait_cb_registered;
	if (!wait_cb_registered) {
		signal(SIGCHLD, wait_cb);
		wait_cb_registered = true;
	}
}
// Execute command in a new background process
void jb_execvp(const char * command, char *const argv[])
{
	if (fork() == 0) { // child:
		errno = 0;
		execvp(command, argv);
		// execlp only returns on error
		exit(1);
	} else // in controlling process:
		init_wait_cb();
}
// Execute command in a new background shell
void jb_shell(const char * command)
{
	if (fork() == 0) { // child:
		errno = 0;
		// from man system(3):
		execl("/bin/sh", "sh", "-c", command, NULL);
		// execl only returns on error
		exit(1);
	} else // in controlling process:
		init_wait_cb();
}
/* Execute command in a new background process.  Note, this does not handle
   quoted arguments properly.  For that, use jb_shell().  This version has the
   advantage of not spawning a shell process.  */
void jb_system(const char * command)
{
	enum { MAXARGS = 64 };
	char * argv[MAXARGS];
	uint8_t argc = 0;
	char * buf = strdup(command);
	for (char * i = strtok(buf, " "); i && argc < MAXARGS;
		i = strtok(NULL, " "), ++argc)
		argv[argc] = i;
	argv[argc] = NULL; /* Set the argument array terminator.  Note that
			      this is not set if command has no arguments
			      (strtok returns NULL on first invocation).  */
#ifdef DEBUG
	fprintf(stderr, "argc is %d\n", argc);
	for (int i = 0; i < argc; ++i) {
		fprintf(stderr, "arg %d: %s\n", i, argv[i]);
	}
#endif//DEBUG
	jb_execvp(argv[0], argv);
	free(buf);
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
