CC := gcc

CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

TARGET := exercise_1
SRC := main.c

.PHONY: all clean cppcheck clang-tidy check

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

cppcheck:
	cppcheck --enable=all --std=c99 --error-exitcode=1 $(SRC)

clang-tidy:
	clang-tidy $(SRC) -- $(CFLAGS)

check: all cppcheck clang-tidy

clean:
	rm -f $(TARGET)