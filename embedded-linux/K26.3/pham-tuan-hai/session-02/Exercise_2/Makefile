all: bin/main

build/stringutils.o: src/stringutils.c
	mkdir -p build
	gcc -fPIC -c src/stringutils.c -o build/stringutils.o
bin/libstringutils.so: build/stringutils.o
	mkdir -p bin
	gcc -shared -o bin/libstringutils.so build/stringutils.o
bin/main: main.c bin/libstringutils.so
	gcc main.c -Iinclude -Lbin -lstringutils -o bin/main -Wl,-rpath=./bin

run: bin/main
	./bin/main "Lap trinh tren Linux"

clean:
	rm -rf bin build
