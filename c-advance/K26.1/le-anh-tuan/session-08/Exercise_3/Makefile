CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := main
SRC := main.c

.PHONY: all clean check

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

check:
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $(SRC)
	clang-tidy $(SRC) -- $(CFLAGS)

clean:
	rm -f $(TARGET)