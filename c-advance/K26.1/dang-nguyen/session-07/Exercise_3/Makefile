CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := log_module
SRC := main.c logging.c
INC := -I.
DEF := -DENABLE_LOGGING -DLOG_LEVEL_MAX=LL_DEBUG

.PHONY: all clean run clang_tidy cpp_check

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INC) $(SRC) $(DEF) -o $(TARGET)

run: $(TARGET)
	$(shell pwd)/$<

clang_tidy: $(SRC)
	clang-tidy $^ -checks='clang-analyzer-*,cert-*,bugprone-*' -- -std=c99 $(INC) $(DEF)

cpp_check: $(SRC)
	cppcheck --enable=all --inconclusive --std=c99 --language=c $(INC) $(DEF) --suppress=missingIncludeSystem $^

clean:
	rm -f $(TARGET)