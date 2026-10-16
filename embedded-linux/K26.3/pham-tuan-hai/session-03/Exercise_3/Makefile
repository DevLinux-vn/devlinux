CFLAGS = -Wall -Wextra -Werror -std=gnu99 -g
all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	gcc $(CFLAGS) -o writer_flock writer_flock.c

writer_fcntl: writer_fcntl.c
	gcc $(CFLAGS) -o writer_fcntl writer_fcntl.c

test_flock:
	./writer_flock "Written from Flock"

test_fcntl:
	./writer_fcntl "Written from Fcntl"

clean:
	rm -rf writer_flock writer_fcntl
