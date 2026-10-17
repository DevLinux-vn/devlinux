CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -Werror -std=c99
BIN_DIR = bin
BIN_NAME = stack_monitor
TARGET  = $(BIN_DIR)/$(BIN_NAME)
SRCS    = main.c

.PHONY: all clean cppcheck clang-tidy run

all: $(TARGET)

$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BIN_DIR)

cppcheck:
	cppcheck --enable=all --inconclusive --std=c99 --error-exitcode=1 --suppress=missingIncludeSystem $(SRCS)

clang-tidy:
	clang-tidy $(SRCS) -- -std=c99
