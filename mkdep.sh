#!/bin/sh
OUT=depend.mk
for SRC in *.c; do
    LINE="$(basename $SRC .c).o: $SRC"
    for DEP in $(grep '#include "' $SRC \
        | cut -d' ' -f2 \
        | tr -d \" ); do
	LINE="$LINE $DEP"
    done
    LINES="$LINES$LINE\n"
done
echo -e $LINES > $OUT
