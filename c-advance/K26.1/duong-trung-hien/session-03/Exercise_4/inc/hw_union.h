#ifndef HW_UNION_H
#define HW_UNION_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @struct hw_reg_bits_t
 * @brief Bit-field representation of a hypothetical 32-bit hardware register.
 *
 * This structure provides individual access to each register field using
 * C bit-fields. It is intended for demonstration purposes only, as the
 * memory layout of bit-fields is implementation-defined and therefore
 * unsuitable for portable hardware register mapping.
 */
typedef struct {
    uint32_t EN   : 1;   /**< Enable bit. */
    uint32_t MODE : 3;   /**< Operating mode field. */
    uint32_t FLAG : 1;   /**< Status flag bit. */
    uint32_t res  : 27;  /**< Reserved bits. */
} hw_reg_bits_t;

/**
 * @union hw_reg_t
 * @brief Peripheral register represented as both a 32-bit value and bit-fields.
 */
typedef union {
    uint32_t      ALL; /**< Complete 32-bit register value. */
    hw_reg_bits_t BIT; /**< Bit-field view of the register. */
} hw_reg_t;

/**
 * @brief Demonstrate access to a hardware register using a peripheral union.
 * @param p_reg Pointer to the hardware register union.
 *
 * @return None.
 */
void peripheral_union(hw_reg_t *p_reg);

#endif /* HW_UNION_H */