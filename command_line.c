// jbwm - Minimalist Window Manager for X
// Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
// See README for license and other details.
#include "command_line.h"
//
#include "config.h"
#include "log.h"
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//
void jbwm_parse_command_line(const int argc, char **argv)
{
  JBWM_LOG("parse_argv(%d,%s...)", argc, argv[0]);
  static const char optstring[] = "1:2:v";
  int8_t opt;
  while((opt = getopt(argc, argv, optstring)) != -1)
    switch (opt) {
    case 'v':
      printf("%s version %s\n", argv[0], VERSION);
      exit(0);
    default:
      printf("%s -[%s]\n", argv[0], optstring);
      exit(1);
    }
}

