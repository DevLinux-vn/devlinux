CC = gcc

all: main

run:
	./main


obj:
	mkdir -p obj

lib:
	mkdir -p lib

mathutil: obj
	$(CC) -c src/mathutils.c -I include -o obj/mathutils.o

mathutil_archive: mathutil lib
	ar rcs lib/libmathutils.a obj/mathutils.o

main:mathutil_archive
	$(CC) app/main.c -I include -Llib -lmathutils -o main


.PHONY: clean
clean:
	rm -r obj/
	rm -r lib/
	rm main
