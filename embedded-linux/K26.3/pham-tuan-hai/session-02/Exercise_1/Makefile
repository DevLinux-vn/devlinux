all: main
dirs:
	mkdir -p obj lib

main: lib/libmathutils.a | dirs
	gcc app/main.c -Iinclude -Llib -lmathutils -o main

lib/libmathutils.a: obj/mathutils.o | dirs
	ar rcs lib/libmathutils.a obj/mathutils.o

obj/mathutils.o: src/mathutils.c | dirs
	gcc -c src/mathutils.c -o obj/mathutils.o

run: main
	./main

clean:
	rm -rf obj/* lib/* main
