#include <stdio.h>
#include "crc_calc.h"
#include "ring_buffer.h"

#define APP_SUCCESS (0)

int32_t main(void) {
    const uint8_t data[] = {0x01, 0x02, 0x03};
    printf("=== CRC-8 of {0x01, 0x02, 0x03} = 0x%02X ===\n", crc8_calc(data, 3));
    
    int val = 0;
    if (ring_buf_push(3) && ring_buf_pop(&val)) {
        printf("=== Ring Buffer: Push 3, Pop %d — OK ===\n", val);
    }
    return APP_SUCCESS;
}