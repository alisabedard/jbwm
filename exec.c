// Copyright 2020, Alisa Bedard <alisabedard@gmail.com>
#include "exec.h"
#include "config.h"
#include <signal.h> // for signal()
#include <stdlib.h>
#include <unistd.h> // for fork(), execl()
void jbwm_exec(const char * command)
{
  if (fork() == 0) { // child
    execl("/bin/sh", "sh", "-c", command, NULL);
    // This is only reached on error.
    exit(1);
  } else // in controlling process:
    signal(SIGCHLD, SIG_IGN);
}
