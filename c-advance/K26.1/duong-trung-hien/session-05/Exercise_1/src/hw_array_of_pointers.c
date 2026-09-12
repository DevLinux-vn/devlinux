#include "hw_array_of_pointers.h"

const char *get_error_string(uint8_t err_code)
{
    if (err_code >= ERR_COUNT)
    {
        return "UNKNOWN_ERROR";
    }
    else
    {
        return p_error_strings[err_code];
    }
}