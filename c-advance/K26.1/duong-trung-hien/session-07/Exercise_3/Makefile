CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99	  
DEF_DEBUG := -DLOG_LEVEL_MAX=LOG_LEVEL_DEBUG
DEF_WARNING := -DLOG_LEVEL_MAX=LOG_LEVEL_WARNING

SRC := test/main.c src/logger_module.c
HDR := inc/logger_module.h
INC := -Iinc

BUILD_DIR := build
TARGET := $(BUILD_DIR)/output
TARGET_DEBUG := $(TARGET)_debug
TARGET_WARNING := $(TARGET)_warn

.PHONY: all all_variants clean rurun_debug run_warn clang_tidy cpp_check

all: $(TARGET_DEBUG)

all_variants: $(TARGET_DEBUG) $(TARGET_WARNING)

$(TARGET_DEBUG): $(SRC) $(HDR) $(BUILD_DIR) 
	$(CC) $(CFLAGS) $(INC) $(SRC) $(DEF_DEBUG) -o $@

$(TARGET_WARNING): $(SRC) $(HDR) $(BUILD_DIR) 
	$(CC) $(CFLAGS) $(INC) $(SRC) $(DEF_WARNING) -o $@

$(BUILD_DIR):
	mkdir -p $@

run_debug: $(TARGET_DEBUG)
	$(shell pwd)/$<

run_warn: $(TARGET_WARNING)
	$(shell pwd)/$<

clang_tidy: $(SRC) $(HDR)
	clang-tidy $(SRC) -- $(INC) -std=c99

cpp_check: $(SRC) $(HDR)
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $^ $(INC)

clean: 
	rm -rf $(BUILD_DIR)
