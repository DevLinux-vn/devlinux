
CC := gcc

CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

TARGET := app

.PHONY: all clean clangtidy cppcheck


all : $(TARGET)


$(TARGET): main.c
	@echo BUILDING MAIN
	$(CC) $(CFLAGS) -o $@ $^


clean:
	rm -rf $(TARGET)

clangtidy:
	clang-tidy main.c -checks='clang-analyzer-*,cert-*,bugprone-*,readability-*' -- 

cppcheck:
	cppcheck --enable=all --inconclusive --std=c99 --language=c --suppress=missingIncludeSystem main.c
