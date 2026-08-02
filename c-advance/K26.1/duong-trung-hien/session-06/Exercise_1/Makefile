CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99 

BUILD_DIR := build
TARGET := $(BUILD_DIR)/output

SRC := test/main.c src/timer_callback.c
HDR := inc/timer_callback.h
INC := -Iinc	

.PHONY = all clean run clang_tidy cpp_check

all: $(TARGET)

$(TARGET): $(SRC) $(HDR) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$<

clang_tidy: $(SRC) $(HDR)
	clang-tidy $(SRC) -- $(INC) -std=c99

cpp_check: $(SRC) $(HDR)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ $(INC)

clean:
	rm -rf $(BUILD_DIR)



