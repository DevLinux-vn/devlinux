CC ?= gcc
FEATURE ?= -DCONFIG_WIFI_ENABLED
CFLAGS ?= -Wall -Wextra -pedantic -Werror -std=c99 -g $(FEATURE)

TARGET = exercise_4
SRCS = main.c
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
	cppcheck --enable=all --suppress=missingIncludeSystem $(FEATURE) --inconclusive --std=c99 $(SRCS)
	clang-tidy $(SRCS) -checks='clang-analyzer-*,cert-*,bugprone-*,-bugprone-easily-swappable-parameters' -- $(CFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
