// Copyright 2016, Jeffrey E. Bedard
#ifndef LIBJB_FILE_H
#define LIBJB_FILE_H
#include <fcntl.h> // for flags
#include <unistd.h> // for close()
typedef int fd_t;
fd_t jb_open(const char * path, const int flags);
#endif//!LIBJB_FILE_H
