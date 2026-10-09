CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lrt -lpthread

all: controller device

controller: controller.o
		$(CC) $(CFLAGS) -o controller controller.o $(LIBS)

device: device.o
		$(CC) $(CFLAGS) -o device device.o $(LIBS)

controller.o: controller.c
		$(CC) $(CFLAGS) -c controller.c

device.o: device.c
		$(CC) $(CFLAGS) -c device.c

clean:
		rm -f *.o controller device