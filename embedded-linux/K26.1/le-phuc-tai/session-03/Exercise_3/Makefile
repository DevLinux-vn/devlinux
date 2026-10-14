CC = gcc
CFLAGS = -Wall

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) writer_fcntl.c -o writer_fcntl

# Chạy thử nghiệm 10 tiến trình flock đồng thời
test_flock: writer_flock
	rm -f system.log
	@echo "🚀 Đang kích hoạt 10 tiến trình ghi ngầm với FLOCK..."
	@for i in $$(seq 1 10); do ./writer_flock "thông điệp flock số $$i" & done; wait
	@echo "📊 Kết quả nghiệm thu (Số dòng trong file log):"
	wc -l system.log
	@echo "🔍 Đếm số lượng nhãn PID hợp lệ:"
	grep -c "^\[PID" system.log

# Chạy thử nghiệm 10 tiến trình fcntl đồng thời
test_fcntl: writer_fcntl
	rm -f system.log
	@echo "🚀 Đang kích hoạt 10 tiến trình ghi ngầm với FCNTL..."
	@for i in $$(seq 1 10); do ./writer_fcntl "thông điệp fcntl số $$i" & done; wait
	@echo "📊 Kết quả nghiệm thu (Số dòng trong file log):"
	wc -l system.log
	@echo "🔍 Đếm số lượng nhãn PID hợp lệ:"
	grep -c "^\[PID" system.log

clean:
	rm -f writer_flock writer_fcntl system.log

.PHONY: all test_flock test_fcntl clean