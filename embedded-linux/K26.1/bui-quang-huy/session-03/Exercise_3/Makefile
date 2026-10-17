CC = gcc
CFLAGS = -Wall -Wextra

TARGETS = writer_flock writer_fcntl

all: $(TARGETS)

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o writer_flock writer_flock.c

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o writer_fcntl writer_fcntl.c

test_flock: writer_flock
	rm -f system.log
	@for i in $$(seq 1 10); do ./writer_flock "message $$i" & done; \
	wait
	@echo "=== KẾT QUẢ KIỂM THỬ FLOCK ==="
	@echo -n "Số lượng dòng thực tế (Kỳ vọng: 10): "
	@wc -l < system.log
	@echo -n "Số dòng có định dạng [PID]... hợp lệ (Kỳ vọng: 10): "
	@grep -c "^\\[PID" system.log

test_fcntl: writer_fcntl
	rm -f system.log
	@for i in $$(seq 1 10); do ./writer_fcntl "message $$i" & done; \
	wait
	@echo "=== KẾT QUẢ KIỂM THỬ FCNTL ==="
	@echo -n "Số lượng dòng thực tế (Kỳ vọng: 10): "
	@wc -l < system.log
	@echo -n "Số dòng có định dạng [PID]... hợp lệ (Kỳ vọng: 10): "
	@grep -c "^\\[PID" system.log

clean:
	rm -f $(TARGETS) system.log

.PHONY: all test_flock test_fcntl clean