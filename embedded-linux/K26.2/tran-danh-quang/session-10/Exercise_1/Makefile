CC ?= gcc
CFLAGS ?= -Wall -Wextra -Werror -pedantic -std=c11 -D_POSIX_C_SOURCE=200809L

TARGETS = iot_server iot_client

all: $(TARGETS)

iot_server: iot_server.o
	$(CC) $(CFLAGS) -o $@ $^

iot_client: iot_client.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c iot_server.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGETS)

.PHONY: all clean
