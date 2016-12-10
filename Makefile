# jbwm - Minimalist Window Manager for X
# Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
# Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
# See README for license and other details.
version=1.54
include config.mk
exe=jbwm
distname=$(exe)-$(version)
# Edit/override this line if you don't want jbwm to install under /usr.
PREFIX=/usr
# Note that $(DESTDIR) is used by the Debian build process.
dest=$(DESTDIR)$(PREFIX)
jbwm_ldflags+=-lX11 
jbwm_cflags+=-DVERSION=\"$(version)\" $(DEBIAN)
jbwm_cflags+=-D_XOPEN_SOURCE=700 -std=c11
objects+=client.o events.o jbwm.o new.o screen.o mwm.o wm_state.o
objects+=button_event.o keys.o util.o max.o select.o snap.o

$(exe): $(objects)
	$(CC) ${jbwm_ldflags} $(LDFLAGS) $(objects) -o $@
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
	make -f Makefile.debug -j 4
small: clean
	CFLAGS='-flto -Os -march=native' ./configure -estx
	make -j4
	cp jbwm jbwm.small.dbg
	make strip
	ls -l jbwm >> small.log
	tail small.log
clang: clean
	make -f Makefile.clang
depend:
	cc -E -MM *.c > depend.mk
#EOF
