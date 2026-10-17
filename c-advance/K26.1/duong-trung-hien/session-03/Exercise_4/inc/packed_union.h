#ifndef PACKED_UNION_H
#define PACKED_UNION_H

#include <stdio.h>
#include <stdint.h>

/**
 * @union unpacked_union_t
 * @brief Unpacked union containing a 32-bit integer and a 5-byte array.
 */
typedef union {
    uint32_t val;   /**< 32-bit integer value. */
    uint8_t  arr[5];/**< 5-byte array. */
} unpacked_union_t;

/**
 * @union packed_union_t
 * @brief Packed union containing a 32-bit integer and a 5-byte array.
 */
typedef union __attribute__((packed)) {
    uint32_t val;   /**< 32-bit integer value. */
    uint8_t  arr[5];/**< 5-byte array. */
} packed_union_t;

/**
 * @struct packed_struct_with_unpacked_union_t
 * @brief Packed structure containing an unpacked union.
 */
typedef struct __attribute__((packed)) {
    unpacked_union_t u; /**< Unpacked union member. */
} packed_struct_with_unpacked_union_t;

/**
 * @struct packed_struct_with_packed_union_t
 * @brief Packed structure containing a packed union.
 */
typedef struct __attribute__((packed)) {
    packed_union_t u; /**< Packed union member. */
} packed_struct_with_packed_union_t;

/**
 * @brief Print the sizes of packed and unpacked union structures.
 *
 * @return None.
 */
void union_size(void);

#endif /* PACKED_UNION_H */