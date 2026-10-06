CC := gcc
# -pthread is strictly required for compiling multithreaded programs in Linux[cite: 4]
CFLAGS := -Wall -Wextra -std=gnu99 -pthread

all: booking

booking: booking.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f booking