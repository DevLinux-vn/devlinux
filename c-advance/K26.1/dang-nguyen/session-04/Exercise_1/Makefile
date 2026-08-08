CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := pixel_app
SRC := main.c console_display.c dummy_display.c
INC := -I.

.PHONY: all clean run clang_tidy cpp_check

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $(TARGET)

run: $(TARGET)
	$(shell pwd)/$<

clang_tidy: $(SRC) dummy_display.h console_display.h i_display.h
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99

cpp_check: $(SRC)
	cppcheck --enable=all --inconclusive --std=c99 --language=c $(INC) --suppress=missingIncludeSystem $^

clean:
	rm -f $(TARGET)