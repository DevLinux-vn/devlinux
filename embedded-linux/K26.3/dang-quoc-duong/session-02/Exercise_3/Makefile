CUR_DIR := .
INC_DIR := ${CUR_DIR}/include
BIN_DIR := ${CUR_DIR}/bin
SRC_DIR := ${CUR_DIR}/src
LIB_DIR := $(CUR_DIR)/lib
BUILD_DIR := ${CUR_DIR}/build


all: create_build create_bin create_lib libs
	gcc -c ${SRC_DIR}/main.c -I${INC_DIR} -o ${BUILD_DIR}/main.o
	gcc ${BUILD_DIR}/main.o -L${LIB_DIR} -lcalc -llogger -o ${BIN_DIR}/main.exe

libs: static shared

static: create_build create_lib
	gcc -c ${SRC_DIR}/calc.c -I${INC_DIR} -o ${BUILD_DIR}/calc.o
	ar rcs ${LIB_DIR}/libcalc.a ${BUILD_DIR}/calc.o

shared: create_build create_lib
	gcc -fPIC -c ${SRC_DIR}/logger.c -I${INC_DIR} -o ${BUILD_DIR}/logger.o
	gcc -shared ${BUILD_DIR}/logger.o -o ${LIB_DIR}/liblogger.so

create_build:
	mkdir -p ${BUILD_DIR}

create_bin:
	mkdir -p ${BIN_DIR}

create_lib:
	mkdir -p ${LIB_DIR}

run: all
	LD_LIBRARY_PATH=${LIB_DIR} ${BIN_DIR}/main.exe

clean:
	rm -rf ${BIN_DIR} ${LIB_DIR} ${BUILD_DIR} main.exe

rebuild: clean all
