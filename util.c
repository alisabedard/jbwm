// Copyright 2016, Jeffrey E. Bedard

#include "util.h"

#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

bool jb_check_errno(const bool val, const char * msg)
{
	if (!val)
		perror(msg);
	return !val;
}

bool jb_check(const bool val, const char * msg)
{
	if (!val)
		fprintf(stderr, "%s\n", msg);
	return !val;
}

void jb_assert(const bool val, const char * msg)
{
	if (jb_check(val, msg))
		exit(1);
}

