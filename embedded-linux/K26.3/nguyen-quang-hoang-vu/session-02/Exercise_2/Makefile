CC = gcc
CFLAGS = -Iinclude -Wall

all: bin/main

build/stringutils.o: src/stringutils.c
	mkdir -p build
	$(CC) $(CFLAGS) -fPIC -c src/stringutils.c -o build/stringutils.o

bin/libstringutils.so: build/stringutils.o
	mkdir -p bin
	$(CC) -shared -o bin/libstringutils.so build/stringutils.o

bin/main: bin/libstringutils.so main.c
	$(CC) $(CFLAGS) main.c -Lbin -lstringutils -o bin/main

run: bin/main
	LD_LIBRARY_PATH=bin ./bin/main "hello world"

clean:
	rm -rf build bin

.PHONY: all run clean
