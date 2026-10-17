CC      := gcc
TARGET  := main
SRC     := main.c
CFLAGS  := -std=gnu11 -Wall -Wextra -O2

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) $(TARGET).exe