CC = gcc
AR = ar
ARFLAGS = rcs
CFLAGS=-Wall -Wextra -std=c99 -Iinclude -g

all: main

run:
	./main

main:
	$(CC) $(CFLAGS) main.c -o main


.PHONY: clean run all
clean:
	rm main
	rm -rf main.dSYM
	rm students.dat
