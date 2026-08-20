CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Werror -pedantic

TARGET := main
SRC := main.c

all:
	$(CC) $(CFLAGS) -DCONFIG_WIFI_ENABLED -o $(TARGET) $(SRC)

wifi:
	$(CC) $(CFLAGS) -DCONFIG_WIFI_ENABLED -o $(TARGET) $(SRC)

ethernet:
	$(CC) $(CFLAGS) -DCONFIG_ETHERNET_ENABLED -o $(TARGET) $(SRC)

both:
	$(CC) $(CFLAGS) \
		-DCONFIG_WIFI_ENABLED \
		-DCONFIG_ETHERNET_ENABLED \
		-o $(TARGET) $(SRC)

none:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all wifi ethernet both none clean