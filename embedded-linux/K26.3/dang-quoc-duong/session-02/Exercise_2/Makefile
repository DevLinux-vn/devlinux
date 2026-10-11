CUR_DIR 	:= .
INC_DIR 	:= ${CUR_DIR}/include
SRC_DIR 	:= ${CUR_DIR}/src
BUILD_DIR 	:= ${CUR_DIR}/build
BIN_DIR 	:= ${CUR_DIR}/bin


all: create_buld	create_bin
	gcc -c ${SRC_DIR}/stringutils.c -I${INC_DIR} -o ${BUILD_DIR}/stringutils.o
	gcc -shared -o ${BIN_DIR}/libstringutils.so ${BUILD_DIR}/stringutils.o
	gcc ${SRC_DIR}/main.c -I${INC_DIR} -L${BIN_DIR} -lstringutils -o ${BIN_DIR}/main.exe
create_buld:
	mkdir -p ${BUILD_DIR}

create_bin:
	mkdir -p ${BIN_DIR}

run: all
	LD_LIBRARY_PATH=${BIN_DIR} ${BIN_DIR}/main.exe "Hello world"

clean:
	rm -rf ${BUILD_DIR}/* ${BIN_DIR}/*
