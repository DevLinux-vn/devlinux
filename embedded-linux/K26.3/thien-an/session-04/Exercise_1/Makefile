CC := gcc
CFLAGS := -Wall -Wextra -std=gnu99

all: manager

manager: manager.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f manager