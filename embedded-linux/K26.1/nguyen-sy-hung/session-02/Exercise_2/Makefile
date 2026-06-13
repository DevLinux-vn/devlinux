CC = gcc

all:
	mkdir -p build bin
	$(CC) -fPIC -c src/stringutils.c -Iinclude -o build/stringutils.o
	$(CC) -shared -o bin/libstringutils.so build/stringutils.o
	$(CC) src/main.c -Iinclude -Lbin -lstringutils -o bin/main

run:
	LD_LIBRARY_PATH=./bin ./bin/main HelloLinux

clean:
	rm -rf build bin