CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean

all: app

app: main.c
	$(CC) $(CFLAGS) main.c -o app

clean:
	rm -f app
