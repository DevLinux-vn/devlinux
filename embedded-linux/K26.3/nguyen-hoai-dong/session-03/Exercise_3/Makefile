all: test_flock test_fcntl
test_flock:
	gcc -o writer_flock writer_flock.c
test_fcntl:
	gcc -o writer_fcntl writer_fcntl.c
clean:
	rm -rf writer_flock writer_fcntl