#!/bin/bash
OUT=depend.mk
for SRC in *.c; do
    LINE="$(basename -s .c $SRC).o: $SRC"
    for DEP in $(grep '#include "' $SRC \
        | cut -d' ' -f2 \
        | tr -d \" ); do
        LINE+=" $DEP"
    done
    LINES+="$LINE\n"
done
echo -e $LINES > $OUT
