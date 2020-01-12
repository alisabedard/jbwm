# jbwm - Minimalist Window Manager for X
# Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.
version=1.58
include config.mk
exe=jbwm
distname=$(exe)-$(version)
# Edit/override this line if you don't want jbwm to install under /usr/local.
PREFIX=/usr/local
SCHEME=mit-scheme
# Note that $(DESTDIR) is used by the Debian build process.
dest=${DESTDIR}${PREFIX}
jbwm_cflags+=-DVERSION=\"$(version)\" $(DEBIAN)
jbwm_cflags+=-D_XOPEN_SOURCE=700 -std=c99
# Titlebar Xft support:
jbwm_cflags+=`pkg-config --cflags xft`
jbwm_ldflags+=`pkg-config --libs xft`
jbwm_ldflags+=-lX11 -lXext
objects+=client.o events.o jbwm.o new.o mwm.o wm_state.o drag.o
objects+=button_event.o keys.o util.o max.o select.o snap.o display.o
objects+=exec.o main.o move_resize.o key_masks.o key_event.o vdesk.o
objects+=geometry.o command_line.o ewmh.o ewmh_state.o ewmh_client.o
objects+=ewmh_wm_state.o title_bar.o font.o shape.o
$(exe): depend.mk $(objects)
	$(CC) ${CFLAGS} ${jbwm_cflags} ${jbwm_ldflags} \
		$(LDFLAGS) $(objects) -o $@
	strip $(exe) -o $(exe).tmp
	ls -l $(exe).tmp >> sz.log
	rm -f $(exe).tmp
	tail sz.log
include depend.mk
# Use `` for basename for BSD make compatibility
${objects}:
	${CC} ${CFLAGS} ${jbwm_cflags} -c `basename $@ .o`.c
strip: 
	# Leave .plt.got
	strip -s \
		-R .jcr \
		-R .eh_frame \
		-R .got \
		-R .note \
		-R .gnu.version \
		-R .note.gnu.build-id \
		-R .note.ABI-tag \
		-R .eh_frame_hdr \
		-R .comment \
		-R .gnu.hash \
		-R .shstrtab \
		$(exe)
INSTALL=install -c
INSTALL_exe=$(INSTALL)
INSTALL_DIR=install -d
install: 
	${INSTALL_DIR} ${dest}/bin ${dest}/share/man/man1 \
		${dest}/share/licenses/${exe}
	${INSTALL_exe} ${exe} ${dest}/bin
	${INSTALL} ${exe}.1 ${dest}/share/man/man1
	${INSTALL} -D -m644 LICENSE "${dest}/share/licenses/${exe}/LICENSE"
clean:
	rm -f $(exe) *.o
distclean: clean
	rm -f *~ *.out .*.swp .*.swn *.orig .*~ *~~
archive: distclean
	cd ..;  tar cJf $(exe)-$(version).tar.xz $(exe)
tags:
	ctags *
indent:
	indent *.[ch]
	sed 's/ \/\//\/\//g' -i'~~' *.[ch]
debug: clean
	./configure -d
	make
small: clean
	CFLAGS='-flto -Os -march=native' ./configure -estx
	make -j4
	cp jbwm jbwm.small.dbg
	make strip
	ls -l jbwm >> small.log
	tail small.log
clang: clean
	make -f Makefile.clang

keys.h: keys.scm keys.txt
	${SCHEME}<keys.scm
	touch keys.h
depend.mk: *.c *.h
	./mkdep.sh
cppcheck:
	cppcheck --enable=all -j 4 -DDEBUG --inline-suppr \
		--inconclusive --std=c11 *.c \
		> /dev/null
#EOF
