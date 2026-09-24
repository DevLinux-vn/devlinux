# Toolchain configuration
CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99 -DUNIT_TEST

# Project paths
INC := -Iinclude -I../../include
PARSER_SRC := ../../src/ipv4_parser.c

# Unit test sources
SRC := main.c
SRC += ut_common.c
SRC += validate_ip_boundary.c
SRC += convert_octet.c
SRC += move_to_next_octet.c
SRC += $(PARSER_SRC)

# Unit test output
BUILD_DIR := ../../build
TARGET := $(BUILD_DIR)/unit_test

.PHONY: all clean run

# Default target: build the unit test executable.
all: $(TARGET)

# Link the integration test with the IPv4 parser static library.
$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(INC) $(CFLAGS) -o $(TARGET) $(SRC)

# Run unit test
run: all
	$(TARGET)

# Remove unit test outputs
clean:
	rm -f $(TARGET)