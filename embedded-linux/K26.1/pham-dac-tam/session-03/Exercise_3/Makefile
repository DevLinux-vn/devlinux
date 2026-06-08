CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LOG_FILE = system.log

TARGETS = writer_flock writer_fcntl
SOURCES = writer_flock.c writer_fcntl.c

.PHONY: all clean test_flock test_fcntl

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o $@ $<

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o $@ $<

# Test flock version
test_flock: writer_flock
	rm -f $(LOG_FILE)
	@echo "Testing flock with 10 concurrent writers..."
	@for i in $$(seq 1 10); do ./writer_flock "message $$i" & done
	@wait
	@echo "Results:"
	@echo -n "Total lines: "; wc -l < $(LOG_FILE)
	@echo -n "Valid log entries: "; grep -c "^\[PID" $(LOG_FILE)

# Test fcntl version
test_fcntl: writer_fcntl
	rm -f $(LOG_FILE)
	@echo "Testing fcntl with 10 concurrent writers..."
	@for i in $$(seq 1 10); do ./writer_fcntl "message $$i" & done
	@wait
	@echo "Results:"
	@echo -n "Total lines: "; wc -l < $(LOG_FILE)
	@echo -n "Valid log entries: "; grep -c "^\[PID" $(LOG_FILE)

# View log file
view:
	@cat $(LOG_FILE) 2>/dev/null || echo "No log file found"

clean:
	rm -f $(TARGETS) $(LOG_FILE) *.o
