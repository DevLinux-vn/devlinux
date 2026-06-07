CC      = gcc
CFLAGS  = -Wall -Wextra
TARGET  = product_mgr

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET) products.dat
