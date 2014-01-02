all: hashed_update

CFLAGS+=-Wall -O3 -D_FILE_OFFSET_BITS=64

hashed_update: hashed_update.o
	${CC} ${LDFLAGS}  hashed_update.o -lcrypto -o hashed_update

