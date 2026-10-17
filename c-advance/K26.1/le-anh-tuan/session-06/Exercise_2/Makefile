CC = gcc

CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

TARGET = exercise_2
SRC = main.c

.PHONY: all clean analyze

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

analyze:
	cppcheck --enable=all --error-exitcode=1 \
		--suppress=missingIncludeSystem \
		--suppress=staticFunction \
		$(SRC)
	clang-tidy $(SRC) -- $(CFLAGS)

clean:
	rm -f $(TARGET) $(TARGET).exe