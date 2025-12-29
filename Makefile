CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
DBGFLAGS = -g -O0 -DDEBUG

all: vm test

vm: main.c glyph.h
	$(CC) $(CFLAGS) main.c -o vm

test: test.c glyph.h
	$(CC) $(CFLAGS) test.c -o test

debug: main.c glyph.h
	$(CC) $(CFLAGS) $(DBGFLAGS) main.c -o vm-debug

clean:
	rm -f vm test vm-debug a.out

.PHONY: all clean debug
