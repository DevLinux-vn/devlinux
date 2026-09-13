CC = gcc
CFLAGS=-Wall -Wextra -std=c99 -Iinclude

all: main verify

run:
	./bin/main

build_dir:
	mkdir -p build

bin_dir:
	mkdir -p bin

sharelib_stringutil: build_dir
	$(CC) $(CFLAGS) -fPIC -c src/stringutils.c  -o build/stringutils.o

linking_stringutil: sharelib_stringutil bin_dir
	$(CC) -shared -o bin/libstringutils.so build/stringutils.o

main: linking_stringutil
	$(CC) $(CFLAGS) app/main.c  -Lbin -lstringutils -o bin/main

verify:
	ldd bin/main

clean:
	rm -r build/
	rm -r bin/

.PHONY: clean run all verify
