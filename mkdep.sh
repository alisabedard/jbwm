#!/bin/sh
OUT=depend.mk
rm -f $OUT
for SRC in *.c; do
    LINE="`basename $SRC .c`.o: $SRC"
    for DEP in `fgrep '#include "' $SRC \
            | cut -d' ' -f2 \
            | tr -d \" `; do
        LINE="$LINE $DEP"
    done
    echo $LINE >> $OUT
done

