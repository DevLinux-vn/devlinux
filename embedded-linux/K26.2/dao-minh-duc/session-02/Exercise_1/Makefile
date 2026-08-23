CC = gcc
CFLAGS = -Iinclude

all: main

obj/mathutils.o: src/mathutils.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c src/mathutils.c -o obj/mathutils.o

lib/libmathutils.a: obj/mathutils.o
	mkdir -p lib
	ar rcs lib/libmathutils.a obj/mathutils.o

main: app/main.c lib/libmathutils.a
	$(CC) $(CFLAGS) app/main.c -Llib -lmathutils -o main

run: main
	./main

clean:
	rm -rf obj lib main
