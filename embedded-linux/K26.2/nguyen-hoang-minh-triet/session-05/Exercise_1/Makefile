#define compiler
CC = gcc

#define compiler flag
CFLAGS = -std=c99 -Wall -pthread -Wextra

#define target
TARGET = booking
SRC = booking.c

#define PHONY
.PHONY = all clean

#define make all
all: $(TARGET)

$(TARGET) : $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

#define make clean
clean:
	rm -f $(TARGET)
 