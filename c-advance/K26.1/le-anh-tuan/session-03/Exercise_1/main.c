/**
 * @file main.c
 * @brief Endianness checker using a union.
 *
 * Hardware endianness describes the byte order used by a processor when storing
 * multi-byte data types in memory. For example, the 32-bit value 0x11223344 is
 * made of four bytes: 0x11, 0x22, 0x33, and 0x44. Endianness determines which
 * of these bytes is stored at the lowest memory address.
 *
 * The Least Significant Byte, or LSB, is the byte with the lowest numerical
 * weight in the value. For 0x11223344, the LSB is 0x44. The Most Significant
 * Byte, or MSB, is the byte with the highest numerical weight. For 0x11223344,
 * the MSB is 0x11.
 *
 * On a Little-Endian system, the LSB is stored at the lowest memory address.
 * Therefore, storing 0x11223344 in memory results in the first byte being 0x44.
 *
 * On a Big-Endian system, the MSB is stored at the lowest memory address.
 * Therefore, storing 0x11223344 in memory results in the first byte being 0x11.
 *
 * Endianness is critically important in embedded systems because hardware
 * registers, sensors, communication buses, and network protocols may define data
 * in a specific byte order. If an embedded engineer reads bytes in the wrong
 * order, sensor values, packet fields, addresses, lengths, and control commands
 * may be interpreted incorrectly. This can cause incorrect measurements,
 * communication failures, unsafe actuator behavior, or corrupted data exchange
 * between systems.
 */

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Union used to inspect the byte order of a 32-bit value.
 */
typedef union
{
    uint32_t full_word;
    uint8_t bytes[4];
} endian_checker_u;

/**
 * @brief Value used to check byte order.
 */
#define ENDIAN_TEST_VALUE (0x11223344UL)

/**
 * @brief Little-endian first byte for ENDIAN_TEST_VALUE.
 */
#define LITTLE_ENDIAN_FIRST_BYTE (0x44U)

/**
 * @brief Big-endian first byte for ENDIAN_TEST_VALUE.
 */
#define BIG_ENDIAN_FIRST_BYTE (0x11U)

/**
 * @brief Program entry point.
 *
 * @return int Program status code.
 */
int main(void)
{
    endian_checker_u endian_checker =
    {
        .full_word = ENDIAN_TEST_VALUE
    };

    printf("=== Endianness Checker ===\n");
    printf("Stored Value: 0x%08lX\n", (unsigned long)endian_checker.full_word);
    printf("First Byte in Memory: 0x%02X\n", endian_checker.bytes[0]);

    if (endian_checker.bytes[0] == LITTLE_ENDIAN_FIRST_BYTE)
    {
        printf("Result: This system is Little-Endian!\n");
    }
    else if (endian_checker.bytes[0] == BIG_ENDIAN_FIRST_BYTE)
    {
        printf("Result: This system is Big-Endian!\n");
    }
    else
    {
        printf("Result: Unknown byte order!\n");
    }


    
    return 0;
}