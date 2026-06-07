CC      = gcc
CFLAGS  = -Wall -Wextra
TARGET  = student_mgr

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET) students.dat
