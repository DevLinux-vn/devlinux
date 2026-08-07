CC = gcc
TARGET = traffic_fsm
SRC = main.c

CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99

.PHONY: all clean analyze compare

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

analyze:
	cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem $(SRC)
	clang-tidy $(SRC) -- $(CFLAGS)

compare: $(TARGET)
	./$(TARGET) > fsm_output.txt
	./$(TARGET) --original > original_output.txt
	cmp fsm_output.txt original_output.txt
	@echo "PASS: FSM output is identical to original switch-case output."
	rm -f fsm_output.txt original_output.txt

clean:
	rm -f $(TARGET) fsm_output.txt original_output.txt