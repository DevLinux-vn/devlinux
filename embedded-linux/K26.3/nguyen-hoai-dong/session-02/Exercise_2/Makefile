all: build bin 
	gcc -fPIC -c src/stringutils.c -o build/stringutils.o
	gcc -shared -o bin/libstringutils.so build/stringutils.o
	gcc main.c -Iinclude -Lbin -lstringutils -o bin/main
run: ./bin/main ./bin/libstringutils.so
	LD_LIBRARY_PATH=./bin ./bin/main
clean:
	rm -rf ./build *.o
	rm -rf ./bin *.so
	rm -rf ./bin main
build:
	mkdir -p build
bin:
	mkdir -p bin