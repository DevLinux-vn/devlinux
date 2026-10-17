CC =	gcc
CFLAGS=	-Wall	-Wextra	-Werror	-std=c11
TARGETS=	monitor-daemon	monitor-cli
.PHONY=	all clean
all:	$(TARGETS)
monitor-daemon:	server.c
	$(CC) $(CFLAGS)	server.c -o monitor-daemon
monitor-cli:	client.c
	$(CC) $(CFLAGS) client.c -o monitor-cli

clean:
	rm -f $(TARGETS)
	rm -f /tmp/monitor.sock

