CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Werror -std=c99
TARGET := mem_layout
SRC := main.c
INC := -I.

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $(TARGET)

check_size: $(TARGET)
	@echo "Size of $<"
	size $<

check_var: $(TARGET)
	@echo "Information about global_init:"
	nm $< | grep -i global_init

	@echo "\n"
	@echo "Information about global_uninit:"
	nm $< | grep -i global_uninit

clean:
	rm -f $(TARGET)