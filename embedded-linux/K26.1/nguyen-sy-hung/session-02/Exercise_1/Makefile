CC = gcc

all:
	mkdir -p obj lib
	$(CC) -c src/mathutils.c -Iinclude -o obj/mathutils.o
	ar rcs lib/libmathutils.a obj/mathutils.o
	$(CC) app/main.c -Iinclude -Llib -lmathutils -o main

run:
	./main

clean:
	rm -rf obj lib main