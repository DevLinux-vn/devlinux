CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := conditional_compilation
SRC := main.c
INC := -I.
DEF := -DCONFIG_WIFI_ENABLED

.PHONY: all clean run clang_tidy cpp_check

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INC) $(SRC) $(DEF) -o $(TARGET)

run: $(TARGET)
	$(shell pwd)/$<

clang_tidy: $(SRC)
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99 $(DEF)

cpp_check: $(SRC)
	cppcheck --enable=all --inconclusive --std=c99 --language=c $(INC) $(DEF) --suppress=missingIncludeSystem $^

clean:
	rm -f $(TARGET)