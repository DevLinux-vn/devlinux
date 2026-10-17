#include "hw_array_of_pointers.h"
#include <stdio.h>

int main()
{
    uint8_t err_code_1 = ERR_TIMEOUT;
    uint8_t err_code_99 = 99;

    printf("Error code %u: %s\n", err_code_1, get_error_string(err_code_1));
    printf("Error code %u: %s\n", err_code_99, get_error_string(err_code_99));

    return 0;
}