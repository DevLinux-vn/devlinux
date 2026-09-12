#include "endianess_checker.h"

endian_type_t endianess_checker(const endian_checker_u *p_checker, uint32_t value)
{
    uint8_t lsb = (uint8_t)((value >> 0)  & 0xFFU);
    uint8_t msb = (uint8_t)((value >> 24) & 0xFFU);

    if (p_checker->bytes[0] == lsb)
    {
        return ENDIAN_LITTLE;
    }
    else if (p_checker->bytes[0] == msb)
    {
        return ENDIAN_BIG;
    }
    else 
    {
        return ENDIAN_UNKNOWN;
    }

}

void endianess_print(const endian_checker_u *p_checker, uint32_t value, endian_type_t type)
{
    printf("=== Endianness Checker ===\n");
    printf("Stored Value: %#x\n", value);
    printf("First Byte in Memory: %#x\n", p_checker->bytes[0]);

    switch (type)
    {
        case ENDIAN_LITTLE:
            printf("Result: This system is Little-Endian!\n");
            break;

        case ENDIAN_BIG:
            printf("Result: This system is Big-Endian!\n");
            break;

        default:
            printf("Result: Unknown endianness\n");
            break;

    }
}