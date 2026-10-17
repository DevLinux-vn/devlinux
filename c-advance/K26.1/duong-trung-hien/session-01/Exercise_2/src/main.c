#include <stdio.h>
#include <stdint.h>
#include "mac_parser.h"

static void print_mac(const uint8_t *mac)
{
    printf("{");

    for (int i = 0; i < 6; i++)
    {
        printf("0x%02X", mac[i]);

        if (i < 5)
        {
            printf(", ");
        }
    }

    printf("}");
}

static void run_test(const char *input)
{
    uint8_t mac_out[6] = {0};
    int8_t result = parse_mac(input, mac_out);

    printf("parse_mac(");

    if (input == NULL)
    {
        printf("NULL");
    }
    else
    {
        printf("\"%s\"", input);
    }

    printf(", mac_out) -> returns %d", result);

    if (result == 0)
    {
        printf(", mac_out = ");
        print_mac(mac_out);
    }

    printf("\n");
}

int main(void)
{
    run_test("00:1A:2B:3C:4D:5E");
    run_test("00-1a-2b-3c-4d-5e");
    run_test("00:1A:2B:3C:4D");
    run_test("00:1A:2B:3C:4D:5E:6F");
    run_test("00:1A:2B:3C:4D:5G");
    run_test(NULL);

    return 0;
}