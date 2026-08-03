CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -Lbin -lstringutils

all: bin/main

bin/main: main.c bin/libstringutils.so | bin
	$(CC) main.c $(CFLAGS) $(LDFLAGS) -o $@

bin/libstringutils.so: build/stringutils.o | bin
	$(CC) -shared -o $@ $<

build/stringutils.o: src/stringutils.c | build
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

build:
	mkdir -p build

bin:
	mkdir -p bin

run: all
	LD_LIBRARY_PATH=$$(pwd)/bin bin/main "hello world"

clean:
	rm -rf build bin

.PHONY: all run clean
