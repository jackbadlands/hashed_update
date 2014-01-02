all: hashed_update

CFLAGS+=-Wall -O3 -D_FILE_OFFSET_BITS=64

hashed_update: hashed_update.o
	${CC} ${LDFLAGS}  hashed_update.o -lcrypto -o hashed_update

# install to DESTDIR/usr/ when called from debuild, 
# but still install to /usr/local when called by just "make install"
DESTDIR=/usr/local
prefix_=${DESTDIR}/usr
prefix=$(shell echo "${prefix_}" | sed 's!local/usr!local!')

install: hashed_update
	mkdir -p ${prefix}/bin/
	install -m 755 hashed_update ${prefix}/bin/
