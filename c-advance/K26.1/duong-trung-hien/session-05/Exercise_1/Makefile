CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

BUILD_DIR := build
TARGET := $(BUILD_DIR)/output

SRC := test/main.c src/hw_array_of_pointers.c
HDR := inc/hw_array_of_pointers.h
INC := -Iinc

.PHONY: all clean run clang_tidy cpp_check

all: $(TARGET)

$(TARGET): $(SRC) $(HDR) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$<

clang_tidy: $(SRC) $(HDR)
	clang-tidy $(SRC) -- $(INC) -std=c99

cpp_check: $(SRC)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ $(INC)

clean:
	rm -rf $(BUILD_DIR)
