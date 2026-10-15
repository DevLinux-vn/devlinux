#include <stdio.h>
#include "stringutils.h"

int main(int argc, char *argv[]){
	if (argc < 2) {
		printf("ERROR");
    return 1;
	}

	char *input_string = argv[1];

	int length = str_count(input_string);
	printf("Do dai cua chuoi: %d\n", length);

	str_to_upper(input_string);
	printf("Chuoi viet hoa: %s\n", input_string);

	str_reverse(input_string);
	printf("Chuoi dao nguoc: %s\n", input_string);

	return 0;
}

