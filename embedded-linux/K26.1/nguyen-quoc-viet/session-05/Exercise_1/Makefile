CC      = gcc
CFLAGS  = -Wall -Wextra -pthread
TARGET  = booking

all: $(TARGET)

$(TARGET): booking.c
	$(CC) $(CFLAGS) -o $(TARGET) booking.c

clean:
	rm -f $(TARGET)
