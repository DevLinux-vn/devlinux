CC       := gcc
TARGET   := logger
SRC      := main.c
LOG_LEVEL ?= LOG_LEVEL_DEBUG

CFLAGS   := -std=gnu11 -Wall -Wextra -O2 \
            -DLOG_LEVEL_MAX=$(LOG_LEVEL)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe