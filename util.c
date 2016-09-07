// Copyright 2016, Jeffrey E. Bedard

// Prevent behavior changes when DEBUG defined from affecting this file.
#undef DEBUG

#include "util.h"

#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void jb_swap(void * a, void * b)
{
	int *c = (int*)a;
	int *d = (int*)b;
	int e = *c;
	*c = *d;
	*d = e;
}

void jb_close(const fd_t fd)
{
	if (close(fd) != -1)
		return; // success
	const uint8_t sz = 32;
	char buf[sz];
	snprintf(buf, sz, "Could not close fd %d", fd);
	perror(buf);
}

fd_t jb_open(const char * path, const int flags)
{
	fd_t r = open(path, flags);
	if (r != -1)
		return r; // success
	const uint8_t sz = 64;
	char buf[sz];
	snprintf(buf, sz, "Could not open %s", path);
	perror(buf);
	return -1;
}

bool jb_check(const bool val, char * msg)
{
	if (!val) {
		char * e = msg ? msg : "Error";
		if (errno)
			perror(e);
		else
			fprintf(stderr, "%s\n", e);
	}
	errno = 0; // reset
	return !val;
}

void jb_assert(const bool val, char * msg)
{
	if (jb_check(val, msg))
		exit(1);
}

bool jb_abort_if_false(bool val, char * msg)
{
	val = jb_check(val, msg);
	if (val)
		abort();
	return val;
}

