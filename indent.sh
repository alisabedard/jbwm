#!/bin/sh
for f in *.[ch]; do vim -s indent.scr $f; done
