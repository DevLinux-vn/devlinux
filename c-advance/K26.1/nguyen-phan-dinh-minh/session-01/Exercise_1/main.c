#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "parse_ipv4.h"

int main(void)
{
    uint32_t ip = 0;

    int8_t ret = parse_ipv4("192.168.1.50", &ip);

    printf("ret = %d\n", ret);
    printf("ip = %u\n", ip);

    return 0;
}
