CC = gcc
CFLAGS = -O3 -static

.PHONY: all clean

all:  libpthread.a(pthread.o)

clean:
	$(RM) *.o libpthread.a
