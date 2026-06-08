all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	gcc writer_flock.c -o writer_flock

writer_fcntl: writer_fcntl.c
	gcc writer_fcntl.c -o writer_fcntl

test_flock:
	rm -f system.log
	@echo "Đang test đồng thời 10 tiến trình với flock..."
	for i in $$(seq 1 10); do ./writer_flock "Thông điệp từ flock $$i" & done; wait
	@echo "Kết quả số dòng trong system.log:"
	wc -l system.log
	@echo "Số dòng hợp lệ định dạng [PID:"
	grep -c "^\[PID" system.log

test_fcntl:
	rm -f system.log
	@echo "Đang test đồng thời 10 tiến trình với fcntl..."
	for i in $$(seq 1 10); do ./writer_fcntl "Thông điệp từ fcntl $$i" & done; wait
	@echo "Kết quả số dòng trong system.log:"
	wc -l system.log
	@echo "Số dòng hợp lệ định dạng [PID:"
	grep -c "^\[PID" system.log

clean:
	rm -f writer_flock writer_fcntl system.log