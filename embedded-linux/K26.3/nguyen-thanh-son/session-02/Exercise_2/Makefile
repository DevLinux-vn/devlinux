CUR_DIR   := .
BIN_DIR   := $(CUR_DIR)/bin
BUILD_DIR := $(CUR_DIR)/build
INC_DIR   := $(CUR_DIR)/include
SRC_DIR   := $(CUR_DIR)/src

LIB_NAME := stringutils
BIN_NAME := main
SAMPLE   := "Hello Embedded Linux"

all: prequisites create_obj create_lib
	gcc $(CUR_DIR)/main.c -I$(INC_DIR) -L$(BIN_DIR) -l$(LIB_NAME) -Wl,-rpath,'$$ORIGIN' -o $(BIN_DIR)/$(BIN_NAME)

prequisites:
	mkdir -p $(BIN_DIR) $(BUILD_DIR)

create_obj:
	gcc -fPIC -c $(SRC_DIR)/stringutils.c -I$(INC_DIR) -o $(BUILD_DIR)/stringutils.o

create_lib:
	gcc -shared -o $(BIN_DIR)/lib$(LIB_NAME).so $(BUILD_DIR)/stringutils.o

run:
	$(BIN_DIR)/$(BIN_NAME) $(SAMPLE)

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BIN_DIR)/*