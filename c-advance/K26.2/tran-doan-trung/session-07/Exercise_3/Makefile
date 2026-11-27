CC ?= gcc
LOG_CFG ?= -DLOG_LEVEL_MAX=LOG_LEVEL_DEBUG
CFLAGS ?= -Wall -Wextra -pedantic -Werror -std=c99 -g $(LOG_CFG)

TARGET = exercise_3
SRCS = main.c logger.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run check

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

check:
	cppcheck --enable=all --suppress=missingIncludeSystem $(LOG_CFG) --inconclusive --std=c99 $(SRCS)
	clang-tidy $(SRCS) -checks='clang-analyzer-*,cert-*,bugprone-*,-bugprone-easily-swappable-parameters' -- $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
