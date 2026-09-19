CC = gcc
AR = ar
CFLAGS=-Wall -Wextra -std=c99 -Iinclude -g
CFLAGS+= -DPRELOAD_TEST_DATA

all: main

run:
	./main

main:
	$(CC) $(CFLAGS) main.c -o main


.PHONY: clean run all
clean:
	rm main
	rm products.dat
