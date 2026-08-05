
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

TARGET = perm_tester

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET)