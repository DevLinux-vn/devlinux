CC      = gcc
CFLAGS  = -Wall -Wextra -g -O2

all: iot_server iot_client

iot_server: iot_server.c iot_server.h
	$(CC) $(CFLAGS) -o $@ iot_server.c

iot_client: iot_client.c iot_server.h
	$(CC) $(CFLAGS) -o $@ iot_client.c

clean:
	rm -f iot_server iot_client

.PHONY: all clean