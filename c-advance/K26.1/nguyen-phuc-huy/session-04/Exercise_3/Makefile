CC := gcc
TARGET := exercise_3
SOURCE := main.c

CFLAGS := -std=c99 \
          -Wall \
          -Wextra \
          -pedantic \
          -Werror

.PHONY: all clean run cppcheck clang-tidy

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

cppcheck:
	cppcheck \
		--enable=all \
		--std=c99 \
		--language=c \
		--suppress=missingIncludeSystem \
		--error-exitcode=1 \
		$(SOURCE)

clang-tidy:
	clang-tidy $(SOURCE) -- $(CFLAGS)

clean:
	rm -f $(TARGET)