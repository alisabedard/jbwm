version=0.2
libjb_cflags+=-Wall -Wextra
libjb_cflags+=-D_XOPEN_SOURCE=700
libjb_cflags+=-DJB_VERSION=${version}
# Note:  Path ordering does matter.
# For FreeBSD build:
libjb_cflags+=-I/usr/local/include
libjb_ldflags+=-L/usr/local/lib
# For NetBSD build:
libjb_cflags+=-I/usr/pkg/include
libjb_ldflags+=-L/usr/pkg/lib -Wl,-R/usr/pkg/lib
libjb_cflags+=-I/usr/X11R7/include
libjb_ldflags+=-L/usr/X11R7/lib -Wl,-R/usr/X11R7/lib
# For OpenBSD build:
libjb_cflags+=-I/usr/X11R6/include
libjb_ldflags+=-L/usr/X11R6/lib -Wl,-R/usr/X11R6/lib
# Libs:
libjb_ldflags+=-lxcb
libjb_cflags+=-fpic
PREFIX=/usr
prog=xstatus
objs=util.o xcb.o time.o file.o
installdir=${DESTDIR}${PREFIX}
libjb_out=libjb.so.1.0.1
libjb: libjb.a libjb.so
libjb.so: ${objs}
	${CC} -shared -Wl,-soname,libjb.so.1 -o ${libjb_out} ${objs} \
		${LDFLAGS} ${libjb_ldflags}
libjb.a: ${objs}
	ar rcs libjb.a ${objs}
${objs}:
	${CC} ${CFLAGS} ${libjb_cflags} -c `basename -s .o $@`.c
include depend.mk
clean:
	rm -f libjb.a *.o
install:
	install -d /usr/lib
	install -d /usr/include/jb
	install libjb.a /usr/lib
	install ${libjb_out} /usr/lib
	install *.h /usr/include/jb
check: libjb.a
	${CC} test.c libjb.a -o test
	./test
depend:
	${CC} -E -MM *.c > depend.mk
