# See LICENSE file for copyright and license details.

include config.mk

SRC = xbmouse.c
OBJ = ${SRC:.c=.o}

all: options xbmouse

options:
	@echo xbmouse build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

xbmouse: xbmouse.o
	@echo CC -o $@
	@${CC} -o $@ xbmouse.o ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f xbmouse ${OBJ} xbmouse-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p xbmouse-${VERSION}
	@cp -R LICENSE README Makefile config.mk \
		xbmouse.1 arg.h ${SRC} xbmouse-${VERSION}
	@tar -cf xbmouse-${VERSION}.tar xbmouse-${VERSION}
	@gzip xbmouse-${VERSION}.tar
	@rm -rf xbmouse-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f xbmouse ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/xbmouse
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < xbmouse.1 > ${DESTDIR}${MANPREFIX}/man1/xbmouse.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/xbmouse.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/xbmouse
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/xbmouse.1

.PHONY: all options clean dist install uninstall
