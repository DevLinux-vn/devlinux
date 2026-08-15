NAME ?= TheAnh
AGE ?= 22

# 1. Trường hợp truyền ĐỦ 2 tham số (chạy thành công)
test_pass:
	@echo "=== TEST ĐỦ THAM SỐ ==="
	./greet.sh $(NAME) $(AGE)

test_fail:
	@echo "=== TEST THIẾU THAM SỐ ==="
	./greet.sh
	./greet.sh $(NAME)