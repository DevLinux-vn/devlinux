#include <stdio.h>
#include <stdint.h>


#include "parse_mac.h"

int main(void)
{
    uint8_t mac[6];

    int8_t ret = parse_mac("00:1A:2B:3C:4D:5E", mac);

    printf("ret = %d\n", ret);

    if (ret == 0)
    {
        printf("MAC = ");

        for (int i = 0; i < 6; i++)
        {
            printf("0x%02X ", mac[i]);

            if ( (i) < 5)
            {
                printf(", ");
            }
        }

        printf("\n");
    }

    return 0;
}