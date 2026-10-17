CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

DEBUG_FLAGS := -g

SRC := main.c
INC := -I.

BUILD_DIR := build
TARGET := $(BUILD_DIR)/output

DEBUG_DIR := deb
TARGET_DB := $(DEBUG_DIR)/debug

.PHONY: all demo run debug clang_tidy cpp_check clean help

all: $(TARGET)

demo: $(TARGET_DB)

$(TARGET): $(SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $@

$(TARGET_DB): $(SRC) $(DEBUG_DIR)
	$(CC) $(DEBUG_FLAGS) $(INC) $(SRC) -o $@

$(BUILD_DIR):
	mkdir -p $@

$(DEBUG_DIR):
	mkdir -p $@

run: $(TARGET)
	$(shell pwd)/$<

debug: $(TARGET_DB)
	gdb $(shell pwd)/$<

clang_tidy: $(SRC)
	clang-tidy $(SRC) -- -std=c99

cpp_check: $(SRC)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ 

clean:
	rm -rf $(BUILD_DIR) $(DEBUG_DIR)

help: 
	@echo "Available targets:"
	@echo "all"
	@echo "demo"
	@echo "run"
	@echo "debug"
	@echo "clang_tidy"
	@echo "cpp_check"
	@echo "clean"

