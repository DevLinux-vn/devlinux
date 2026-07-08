#ifndef ENDIANESS_CHECKER_H
#define ENDIANESS_CHECKER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Enumeration of supported endianness types.
 *
 * This enumeration represents the possible byte-order formats
 * detected by the endianness checker.
 */
typedef enum {
    ENDIAN_UNKNOWN = 0,
    ENDIAN_LITTLE,
    ENDIAN_BIG
} endian_type_t;

/**
 * @brief Union used to inspect byte-level representation of a 32-bit value.
 *
 * This union allows accessing the same 32-bit unsigned integer in two ways:
 * - As a full 32-bit word (full_word)
 * - As an array of 4 individual bytes (bytes)
 *
 * It is commonly used for checking system endianness by examining
 * the ordering of bytes in memory.
 */
typedef union {
    uint32_t full_word;
    uint8_t  bytes[4];
} endian_checker_u;

/**
 * @brief Checks and analyzes the endianness representation of a 32-bit value.
 *
 * This function stores the given 32-bit value into a union pointed
 * by p_checker and allows inspection of its byte-level representation in memory.
 * It is typically used to determine whether the system is little-endian or big-endian.
 *
 * @param p_checker Pointer to an endian_checker_u union used to access byte representation.
 *                  Must not be NULL.
 * @param value     32-bit unsigned integer value to be analyzed.
 *
 * @return The detected endianness as an endian_type_t value.
 */
endian_type_t endianess_checker(const endian_checker_u *p_checker, uint32_t value);

/**
 * @brief Prints the result of the endianness check.
 *
 * This function displays the input value, the first byte stored in memory,
 * and the detected endianness in a human-readable format.
 *
 * @param p_checker Pointer to an initialized endian_checker_u union.
 *                  Must not be NULL.
 * @param value     32-bit unsigned integer that was analyzed.
 * @param type      Endianness type returned by endianess_checker().
 *
 * @return None
 */
void endianess_print(const endian_checker_u *p_checker, uint32_t value, endian_type_t type);

#endif