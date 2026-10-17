CC := gcc

CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

TARGET := exercise_1
SRC := main.c

.PHONY: all clean analyze

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

analyze:
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $(SRC)
	clang-tidy $(SRC) -- -std=c99 -Wall -Wextra -pedantic -Werror

clean:
	rm -f $(TARGET) $(TARGET).exe