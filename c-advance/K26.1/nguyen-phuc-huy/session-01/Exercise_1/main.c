
#include <stdio.h>
#include <stdint.h>
#include "ipv4_parser.h"


#define APP_SUCCESS    (0u)
#define APP_FAILURE    (-1)


#define ARG_NUM        (2)

int32_t main(int32_t argc, char *argv[])
{
    uint32_t ip_out = 0;
    int8_t ret = 0;
    if(ARG_NUM != argc )
    {
        printf("Usage: %s <ipv4_address>\n", argv[0]);
        printf("Example: %s 192.168.1.50\n", argv[0]);
        return APP_FAILURE;
    }
    else
    {
        ret = parse_ipv4(argv[1], &ip_out);
        if(APP_SUCCESS == ret)
        {
            printf("Parse successful: %s -> 0x%X\n", argv[1], ip_out);
        }
        else
        {
            printf("Parse failed: %s\n", argv[1]);
        }
    }
    return ret;
}
