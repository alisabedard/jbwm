version=0.2
CFLAGS+=-Wall -Wextra
CFLAGS+=-D_XOPEN_SOURCE=700
CFLAGS+=-DJB_VERSION=${version}
# Note:  Path ordering does matter.
# For FreeBSD build:
CFLAGS+=-I/usr/local/include
LDFLAGS+=-L/usr/local/lib
# For NetBSD build:
CFLAGS+=-I/usr/pkg/include
LDFLAGS+=-L/usr/pkg/lib -Wl,-R/usr/pkg/lib
CFLAGS+=-I/usr/X11R7/include
LDFLAGS+=-L/usr/X11R7/lib -Wl,-R/usr/X11R7/lib
# For OpenBSD build:
CFLAGS+=-I/usr/X11R6/include
LDFLAGS+=-L/usr/X11R6/lib -Wl,-R/usr/X11R6/lib
# Libs:
LDFLAGS+=-lxcb
PREFIX=/usr
prog=xstatus
objs=util.o xcb.o time.o file.o
installdir=${DESTDIR}${PREFIX}
libjb.a: ${objs}
	ar rcs libjb.a ${objs}
include depend.mk
clean:
	rm -f libjb.a *.o
install:
	install -d /usr/lib
	install -d /usr/include/jb
	install libjb.a /usr/lib
	install *.h /usr/include/jb
check: libjb.a
	${CC} test.c libjb.a -o test
	./test
depend:
	${CC} -E -MM *.c > depend.mk
