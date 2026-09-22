all: create_dirs build

create_dirs:
	mkdir -p build/ bin/

build: create_dirs
	gcc -c -fPIC src/stringutils.c -Iinclude -o build/stringutils.o
	gcc -shared -o bin/libstringutils.so build/stringutils.o
	gcc src/main.c -Iinclude -Lbin -lstringutils -o bin/main

run : all
	LD_LIBRARY_PATH=./bin ./bin/main "Hello"

verify:
	LD_LIBRARY_PATH=./bin ldd bin/main

clean:
	rm -rf build/ bin/