CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

all: memory_analyzer

memory_analyzer: main.c
	$(CC) $(CFLAGS) main.c -o memory_analyzer

clean:
	rm -f memory_analyzer