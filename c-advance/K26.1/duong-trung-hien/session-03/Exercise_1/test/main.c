#include "endianess_checker.h"

/**
1. What does hardware "Endianness" mean in terms of memory addresses?
    - Hardware Endianness refers to the order in which a computer stores multi-byte data (like integers) in memory addresses. 
    - It defines whether the least significant byte (LSB) or the most significant byte (MSB) is stored at the lowest memory address.
    - So, endianness is basically about byte ordering in memory.
2. What do LSB (Least Significant Byte) and MSB (Most Significant Byte) mean in this context?
    - The MSB is the byte that has the highest value contribution in a multi-byte number.
    + It is the “most important” part of the number.
    + It represents the highest-order bits.
    + Changing it has the biggest impact on the value.

    - The LSB is the byte that has the lowest value contribution in a multi-byte number.
    + It is the “least important” part of the number.
    + It represents the lowest-order bits.
    + Changing it only slightly affects the value.
3. Why is it critically important for embedded engineers to understand endianness when transmitting data over a network or 
reading from a hardware sensor?
    For embedded engineers, understanding endianness is critically important when transmitting data over a network or reading from a 
hardware sensor because different systems may interpret byte order differently, which can lead to incorrect data interpretation.

Key reasons:
    1. Data consistency across systems
    - Networks often follow big-endian (network byte order).
    - Many embedded systems use little-endian.
    - Without conversion, a multi-byte value (e.g., 0x12345678) may be interpreted incorrectly on the receiving side.
    2. Sensor data correctness
    - Hardware sensors (ADC, IMU, etc.) may output multi-byte values in a fixed byte order.
    - If the MCU assumes the wrong endianness, values like temperature, pressure, or acceleration will be wrong or completely corrupted.
    3. Interoperability
    - Embedded devices often communicate with:
    + PCs
    + Other MCUs
    + Communication protocols (CAN, TCP/IP, SPI, I2C)
    - Each may use different byte ordering rules, so understanding endianness ensures correct integration.
    4. Debugging and reliability
    - Many “random wrong values” bugs in embedded systems come from endianness mismatch, making it essential knowledge for debugging.

 */
int main()
{
    endian_checker_u *p_checker = malloc(sizeof(endian_checker_u));
    
    if(p_checker != NULL)
    {
        p_checker->full_word = 0x11223344;
        uint8_t result = endianess_checker(p_checker, p_checker->full_word);
        endianess_print(p_checker, p_checker->full_word, result);
    }
    else
    {
        return -1;
    }

    free(p_checker);
    return 0;
}