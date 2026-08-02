CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

SRC := main.c
INC := -I.

BUILD_DIR := build
TARGET := $(BUILD_DIR)/output

all: $(TARGET)

prep: $(SRC)
	$(CC) -E $^

$(TARGET): $(SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $@

$(BUILD_DIR): 
	mkdir -p $@

run: $(TARGET)
	$(shell pwd)/$<

clang_tidy: $(SRC) $(HDR)
	clang-tidy $(SRC) -- $(INC) -std=c99

cpp_check: $(SRC) $(HDR)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ $(INC)

clean: 
	rm -rf $(BUILD_DIR)
