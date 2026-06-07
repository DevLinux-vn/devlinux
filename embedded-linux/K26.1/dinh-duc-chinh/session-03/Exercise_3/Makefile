CC      = gcc
CFLAGS  = -Wall -Wextra

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) $< -o $@

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) $< -o $@

# Stress test: 50 process song song, mỗi process ghi 1 dòng
test_flock: writer_flock
	rm -f system.log
	for i in $$(seq 1 50); do ./writer_flock "msg from $$i" & done; wait
	@echo "--- system.log (flock) ---"
	@cat system.log
	@echo "Lines: $$(wc -l < system.log)  (expected 50)"

test_fcntl: writer_fcntl
	rm -f system.log
	for i in $$(seq 1 50); do ./writer_fcntl "msg from $$i" & done; wait
	@echo "--- system.log (fcntl) ---"
	@cat system.log
	@echo "Lines: $$(wc -l < system.log)  (expected 50)"

clean:
	rm -f writer_flock writer_fcntl system.log

.PHONY: all test_flock test_fcntl clean
