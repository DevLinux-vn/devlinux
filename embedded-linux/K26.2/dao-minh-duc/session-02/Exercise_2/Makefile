CC = gcc
CFLAGS = -Iinclude
CC = gcc
CFLAGS = -Iinclude

CC = gcc
CFLAGS = -Iinclude

all: bin/main

build/stringutils.o: src/stringutils.c
	mkdir -p build
	$(CC) -fPIC $(CFLAGS) -c src/stringutils.c -o build/stringutils.o

bin/libstringutils.so: build/stringutils.o
	mkdir -p bin
	$(CC) -shared -o bin/libstringutils.so build/stringutils.o

bin/main: src/main.c bin/libstringutils.so
	$(CC) $(CFLAGS) src/main.c -Lbin -lstringutils -o bin/main

run: bin/main
	LD_LIBRARY_PATH=bin ./bin/main "hello world"

clean:
	rm -rf build bin
