CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99

TARGET := main
SRCS := main.c
OBJS := $(SRCS:.c=.o)


.PHONY: all clean cppcheck clangtidy
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

clangtidy: 
	@echo "Running clang-tidy static analysis..."
	clang-tidy main.c -checks='clang-analyzer-*,cert-*,bugprone-*,readability-*' -- 

cppcheck:
	@echo "Running cppcheck static analysis..."
	cppcheck --enable=all --inconclusive --std=c99 --language=c --suppress=missingIncludeSystem main.c