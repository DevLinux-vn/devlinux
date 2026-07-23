

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Application success return code.
 */
#define APP_SUCCESS (0)

/**
 * @brief Test value used to check byte order.
 */
#define ENDIAN_TEST_VALUE (0x11223344UL)

/**
 * @brief Number of bytes in a 32-bit word.
 */
#define WORD_SIZE_BYTES (4U)

/**
 * @brief Union used to inspect byte order of a 32-bit value.
 */
typedef union
{
    uint32_t full_word;
    uint8_t bytes[WORD_SIZE_BYTES];
} endian_checker_u;



/************************COMMENT OUTPUT *****************************/
/*=== Endianness Checker ===
Stored Value: 0x11223344
First Byte in Memory: 0x44
Result: This system is Little-Endian!*/




/**
 * @brief Program entry point.
 *
 * @return APP_SUCCESS when the program completes successfully.
 */
int main(void)
{
    endian_checker_u checker = { 0U };

    checker.full_word = (uint32_t)ENDIAN_TEST_VALUE;

    (void)printf("=== Endianness Checker ===\n");
    (void)printf("Stored Value: 0x%08X\n", (unsigned int)checker.full_word);
    (void)printf("First Byte in Memory: 0x%02X\n", (unsigned int)checker.bytes[0]);

    if (checker.bytes[0] == 0x44U)
    {
        (void)printf("Result: This system is Little-Endian!\n");
    }
    else if (checker.bytes[0] == 0x11U)
    {
        (void)printf("Result: This system is Big-Endian!\n");
    }
    else
    {
        (void)printf("Result: Unknown endianness!\n");
    }

    return APP_SUCCESS;
}
