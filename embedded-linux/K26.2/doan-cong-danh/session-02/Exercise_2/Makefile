CC = gcc
CFLAG = -Wall -Werror -fPIC -Iinclude -std=gnu99
.PHONY: clean run verify all
BIN_DIR = bin
BUILD_DIR = build


BIN = $(BIN_DIR)/libstringutils.so
BUILD = $(BUILD_DIR)/stringutils.o

$(BUILD): src/stringutils.c include/stringutils.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAG) -c src/stringutils.c -o $(BUILD)
$(BIN): $(BUILD)
	mkdir -p $(BIN_DIR)
	$(CC) -shared -o $(BIN_DIR)/libstringutils.so $(BUILD_DIR)/stringutils.o

TARGET = $(BIN_DIR)/main 
$(TARGET): main.c $(BIN) 
	$(CC) main.c -Iinclude -L$(BIN_DIR) -lstringutils -o $(TARGET)
	
verify: $(TARGET)
	ldd $(TARGET)

all: $(TARGET) 

run: $(TARGET)
	LD_LIBRARY_PATH=$(BIN_DIR) ./$(TARGET) "cong danh"

clean:
	rm -rf $(BIN) $(BUILD) $(BIN_DIR)/main