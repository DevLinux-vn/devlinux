CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

SRC := main.c

BUILD_DIR := build
TARGET := $(BUILD_DIR)/output

.PHONY: all clean run clang_tidy cpp_check

all: $(TARGET)

$(TARGET): $(SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	$(shell pwd)/$<

clang_tidy: $(SRC)
	clang-tidy $(SRC) -- -std=c99

cpp_check: $(SRC)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ 

clean: 
	rm -rf $(BUILD_DIR)
