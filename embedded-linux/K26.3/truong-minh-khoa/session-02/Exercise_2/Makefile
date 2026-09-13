CC = gcc

all: main verify

run:
	./bin/main

build_dir:
	mkdir -p build

bin_dir:
	mkdir -p bin

sharelib_stringutil: build_dir
	$(CC) -fPIC -c src/stringutils.c -Iinclude -o build/stringutils.o

linking_stringutil: sharelib_stringutil bin_dir
	$(CC) -shared -o bin/libstringutils.so build/stringutils.o

main:linking_stringutil
	$(CC) app/main.c -Iinclude -Lbin -lstringutils -o bin/main

verify:
	ldd bin/main

.PHONY: clean
clean:
	rm -r build/
	rm -r bin/
