CC = gcc

FLOCK = writer_flock
FCNTL = writer_fcntl


all: $(FLOCK) $(FCNTL)


$(FLOCK): writer_flock.c
	$(CC) writer_flock.c -o $(FLOCK)


$(FCNTL): writer_fcntl.c
	$(CC) writer_fcntl.c -o $(FCNTL)


test_flock: $(FLOCK)
	./writer_flock "flock test message"


test_fcntl: $(FCNTL)
	./writer_fcntl "fcntl test message"


clean:
	rm -f writer_flock writer_fcntl system.log