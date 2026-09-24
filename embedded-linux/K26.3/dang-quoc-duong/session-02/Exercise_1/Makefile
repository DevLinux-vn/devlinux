CUR_DIR := .
INC_DIR := ${CUR_DIR}/include
OBJ_DIR := ${CUR_DIR}/obj
APP_DIR := ${CUR_DIR}/app
SRC_DIR := ${CUR_DIR}/src
LIB_DIR := $(CUR_DIR)/lib


all: create_obj create_lib
	gcc -c ${SRC_DIR}/mathutils.c -o ${OBJ_DIR}/mathutils.o
	ar rcs ${LIB_DIR}/libmathutils.a ${OBJ_DIR}/mathutils.o
	gcc ${APP_DIR}/main.c -Iinclude -Llib -lmathutils -o main.exe

create_obj:
	mkdir -p ${OBJ_DIR}

create_lib:
	mkdir -p ${LIB_DIR}

run: main.exe
	./main.exe

clean:
	rm -rf ${OBJ_DIR}
	rm -rf ${LIB_DIR}
	rm -rf main.exe
