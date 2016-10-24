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
libjb_so=libjb.so.1.0.0
libjb: libjb.so libjb.a
libjb.so: ${objs}
	${CC} -shared -Wl,-soname,libjb.so -o ${libjb_so} ${objs} \
		${LDFLAGS} ${libjb_ldflags}
libjb.a: ${objs}
	ar rcs libjb.a ${objs}
${objs}:
	${CC} ${CFLAGS} ${libjb_cflags} -c `basename -s .o $@`.c
include depend.mk
clean:
	rm -f ${libjb_so} libjb.a *.o
libdir=${DESTDIR}${PREFIX}/lib
incdir=${DESTDIR}${PREFIX}/include/libjb
INSTALL=install
install:
	${INSTALL} -d ${libdir} ${incdir}
	install ${libjb_so} ${libdir}
	install *.h ${incdir}
uninstall:
	rm -rf ${incdir}
	rm -f ${libdir}/${libjb_so}
check: libjb.a
	${CC} test.c libjb.a -o test
	./test
depend:
	${CC} -E -MM *.c > depend.mk
