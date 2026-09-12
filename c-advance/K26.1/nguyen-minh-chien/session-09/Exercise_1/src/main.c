/**
 * @file    main.c
 * @brief   Demo application linking the CRC and Ring Buffer libraries.
 */
#include <stdio.h>
#include "crc_calc.h"
#include "ring_buffer.h"

/**
 * @brief   Program entry point.
 *
 * @return  0 on success.
 */
int main(void)
{
    const uint8_t data[] = {0x01, 0x02, 0x03};
    int val = 0;

    printf("=== CRC-8 of {0x01, 0x02, 0x03} = 0x%02X ===\n",
           crc8_calc(data, 3U));

    if (ring_buf_push(3) && ring_buf_pop(&val))
    {
        printf("=== Ring Buffer: Push 3, Pop %d \xe2\x80\x94 OK ===\n", val);
    }

    return 0;
}