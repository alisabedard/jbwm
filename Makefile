# jbwm - Minimalist Window Manager for X
# Copyright 2008-2020, Alisa Bedard <alisabedard@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <evilwm@6809.org.uk>
# See README for license and other details.
version=1.64
# PREFIX may be overridden in config.mk
include config.mk
exe=jbwm
distname=$(exe)-$(version)
#SCHEME=mit-scheme
SCHEME=echo
# Note that $(DESTDIR) is used by the Debian build process.
dest=${DESTDIR}${PREFIX}
jbwm_cflags+=-DVERSION=\"$(version)\" $(DEBIAN)
jbwm_cflags+=-D_XOPEN_SOURCE=700 -std=c99
jbwm_ldflags+=-lX11 -lXext -lpthread
objects+=client.o events.o jbwm.o new.o mwm.o wm_state.o drag.o
objects+=button_event.o keys.o util.o max.o select.o snap.o display.o
objects+=exec.o main.o move_resize.o key_event.o vdesk.o draw.o
objects+=geometry.o command_line.o ewmh.o ewmh_state.o ewmh_client.o
objects+=ewmh_wm_state.o title_bar.o font.o shape.o atom.o ewmh_client_list.o
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
depend.mk: *.c *.h
	chmod 755 mkdep.sh
	./mkdep.sh
cppcheck:
	cppcheck --enable=all -j 4 -DDEBUG --inline-suppr \
		--inconclusive --std=c11 *.c \
		> /dev/null
check: small
	strip jbwm
	ls -l jbwm
distcheck: archive
	ls -l | grep jbwm
#EOF
