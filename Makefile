CFLAGS=-ansi -Wpedantic -Wall -Werror -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread
LFLAGS=

all: setta.exe

setta.exe: setta.o DBGpthread.o
	gcc ${LFLAGS} -o setta.exe setta.o DBGpthread.o ${LIBRARIES}

setta.o: setta.c DBGpthread.h
	gcc -c ${CFLAGS} setta.c

DBGpthread.o: DBGpthread.c printerror.h
	gcc -c ${CFLAGS} DBGpthread.c

.PHONY: clean run

clean:
	rm -f *.exe *.o *~ core

run: setta.exe
	./setta.exe


