CC = gcc
AR = ar
ARFLAGS = rcs
CFLAGS=-Wall -Wextra -std=c99 -Iinclude -g
# CFLAGS+= -DPRELOAD_STUDENT_LIST

all: main

run:
	./main

main:
	$(CC) $(CFLAGS) main.c -o main


.PHONY: clean run all
clean:
	rm main
	rm students.dat
