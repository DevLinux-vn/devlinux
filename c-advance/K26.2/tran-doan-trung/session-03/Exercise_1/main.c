// Discussion Task
// 1.
// Q: What does hardware "Endianness" mean in terms of memory addresses?
// A: Endianess means the order in which bytes are arranged within larger data types (like integers) when stored in memory. There are two main types of endianness:
// - Big-endian: The most significant byte (MSB) is stored at the lowest memory address, and the least significant byte (LSB) is stored at the highest memory address. For example, the 32-bit integer 0x12345678 would be stored as 0x12, 0x34, 0x56, 0x78 in memory.
// - Little-endian: The least significant byte (LSB) is stored at the lowest memory address, and the most significant byte (MSB) is stored at the highest memory address. For example, the 32-bit integer 0x12345678 would be stored as 0x78, 0x56, 0x34, 0x12 in memory.
// 2. What do LSB (Least Significant Byte) and MSB (Most Significant Byte) mean in this context?
// LSB mean Least Significant Bit, which is the bit in a binary number that represents the smallest value. It is the rightmost bit in a binary representation. For example, in the binary number 1010, the LSB is 0.
// MSB means Most Significant Bit, which is the bit in a binary number that represents the largest value. It is the leftmost bit in a binary representation. For example, in the binary number 1010, the MSB is 1. 
// 3. Why is it critically important for embedded engineers to understand endianness when transmitting data over a network or reading from a hardware sensor?
// It is critically important for embedded engineers to understand endianness because different systems may use different byte orders. When transmitting data over a network or reading from a hardware sensor, if the sender and receiver have different endianness, the data may be misinterpreted, leading to incorrect results or system failures. For example, if a big-endian system sends a 32-bit integer to a little-endian system without proper conversion, the receiving system may interpret the bytes incorrectly, resulting in unexpected values. Therefore, understanding and handling endianness is essential for ensuring data integrity and compatibility between different systems.

#include <stdint.h>
#include <stdio.h>

typedef union {
    uint32_t full_word;
    uint8_t  bytes[4];
} endian_checker_u;

int main(void) {
    endian_checker_u endian_checker = {.full_word = 0x11223344};
    printf("=== Endianness Checker ===\n");
    printf("Stored Value: 0x%08X\n", endian_checker.full_word);
    printf("First Byte in Memory: 0x%02X\n", endian_checker.bytes[0]);
    if (endian_checker.bytes[0] == 0x11) {
        printf("System is Big-endian\n");
    } else if (endian_checker.bytes[0] == 0x44) {
        printf("System is Little-endian\n");
    } else {
        printf("Unknown endianness\n");
    }
    return 0;
}