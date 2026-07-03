#include <stdio.h>
#include <stdint.h>

#include "ipv4_parser.h"

static void test_ipv4(const char *ip_str)
{
    uint32_t ip_out = 0U;
    int8_t ret = parse_ipv4(ip_str, &ip_out);

    if (ip_str != NULL)
    {
        printf("Input : %s\n", ip_str);
    }
    else
    {
        printf("Input : NULL\n");
    }

    printf("Return: %d\n", ret);

    if (ret == 0)
    {
        printf("IP    : 0x%08X (%u)\n", ip_out, ip_out);
    }

    printf("----------------------------\n");
}

int main(void)
{
    test_ipv4("192.168.1.50");
    test_ipv4("0.0.0.0");
    test_ipv4("255.255.255.255");

    test_ipv4("256.0.0.1");
    test_ipv4("192.168.1");
    test_ipv4("192.168.1.");
    test_ipv4("192..168.1");
    test_ipv4(".192.168.1");
    test_ipv4("192.168.1.1.1");

    test_ipv4(NULL);

    return 0;
}