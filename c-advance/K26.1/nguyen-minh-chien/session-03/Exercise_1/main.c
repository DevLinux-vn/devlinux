#include <stdint.h>
#include <stdio.h>
/**
 * @file main.c
 * @brief Endianness checker using a union of uint32_t and uint8_t array.
 *
 * @details
 * 1. Endianness: defines which byte of a multi-byte value is stored
 *    at the lowest memory address. Little-Endian stores the LSB
 *    first; Big-Endian stores the MSB first.
 *
 * 2. LSB/MSB: for 0x11223344, MSB = 0x11 (most significant byte),
 *    LSB = 0x44 (least significant byte).
 *
 * 3. Importance for embedded engineers: systems with different
 *    endianness will misinterpret multi-byte data if byte order is
 *    not converted, causing wrong values over network communication
 *    or when reading multi-byte sensor data (I2C/SPI).
 */
typedef union {
    uint32_t full_word;
    uint8_t  bytes[4];
} endian_checker_u;

static const uint32_t TEST_PATTERN = 0x11223344U;
static const uint8_t  LITTLE_ENDIAN_LSB = 0x44U;

int main(void){
    endian_checker_u var = {0};
    var.full_word = TEST_PATTERN;
    printf("=== Endianness Checker ===\n");
    printf("Store value: 0x%08X\n", (unsigned int)var.full_word);
    printf("First byte in memory :0x%02X\n", var.bytes[0]);
    if ( var.bytes[0] == LITTLE_ENDIAN_LSB ) {
        printf("Result: This system is Little-Endian!\n");
    }
    else
    {
        printf("Result: This system is Big-Endian!\n");
    }
    return 0;
}