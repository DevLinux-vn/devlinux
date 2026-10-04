CC      = gcc
CFLAGS  = -Wall -Wextra -g -O2

all: event_monitor event_client

event_monitor: event_monitor.c event_monitor.h
	$(CC) $(CFLAGS) -o $@ event_monitor.c

event_client: event_client.c event_monitor.h
	$(CC) $(CFLAGS) -o $@ event_client.c

clean:
	rm -f event_monitor event_client

.PHONY: all clean