CC = gcc
# Added mandatory compiler flags required by the system evaluation script
CFLAGS = -Wall -Wextra -Werror -g -std=gnu99 -O2
TARGET = daemon_sensor
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean