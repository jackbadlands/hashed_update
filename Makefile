all: hashed_update

hashed_update: hashed_update.o
	${CC} ${LDFLAGS} hashed_update.o -lcrypto -o hashed_update
