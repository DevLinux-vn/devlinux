CC = gcc
CFLAGS = -Iinclude -Wall -fPIC

all: bin/main

build/stringutils.o: src/stringutils.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c src/stringutils.c -o build/stringutils.o

bin/libstringutils.so: build/stringutils.o
	@mkdir -p bin
	$(CC) -shared -o bin/libstringutils.so build/stringutils.o

bin/main: main.c bin/libstringutils.so
	@mkdir -p bin
	$(CC) -Iinclude main.c -Lbin -lstringutils -o bin/main

run: all
	@LD_LIBRARY_PATH=./bin ./bin/main "hello linux"

clean:
	rm -rf build bin