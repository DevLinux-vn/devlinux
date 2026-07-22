#include <stdio.h>
#include <stdint.h>

#define UNION_ARR_SIZE 5U
typedef union {
    uint32_t val;
    uint8_t  arr[UNION_ARR_SIZE];
} unpacked_union_t;

typedef struct __attribute__((packed)) {
    unpacked_union_t u;
} packed_struct_with_unpacked_union_t;

typedef union __attribute__((packed))
{
    uint32_t val;
    uint8_t  arr[UNION_ARR_SIZE];
} packed_union_t;

typedef struct __attribute__((packed))
{
    packed_union_t u;
} packed_struct_with_packed_union_t;

typedef struct {
    uint32_t EN   : 1;
    uint32_t MODE : 3;
    uint32_t FLAG : 1;
    uint32_t res  : 27;
} hw_reg_bits_t;

typedef union {
    uint32_t      ALL;
    hw_reg_bits_t BIT;
} hw_reg_t;
/**
 * @brief print size of struct has packed and unpacked union
 *        to compare size of them
 */
static void packed_union_task(void);
/**
 * @brief Print the size of the hw_reg_bits_t bit-field struct, then
 *        demonstrate the Peripheral Union Pattern: set the EN bit via
 *        the bit-field member, and clear the whole register via the
 *        ALL member.
 */
static void hw_register_task(void);
int main(void) {
    packed_union_task();
    hw_register_task();
    return 0;
}

static void packed_union_task(void)
{
    printf("=== Advanced Nested Packing ===\n");
    printf("Size of struct with UNPACKED union: %zu bytes (tail padding exists)\n",
           sizeof(packed_struct_with_unpacked_union_t));
    printf("Size of struct with PACKED union: %zu bytes (tail padding eliminated)\n",
           sizeof(packed_struct_with_packed_union_t));
}
static void hw_register_task(void)
{
    hw_reg_t reg = { 0 };

    printf("=== Struct Bit-Fields & Hardware Mapping ===\n");
    printf("Size of hw_reg_bits_t: %zu bytes\n", sizeof(hw_reg_bits_t));

    printf("Register ALL before: 0x%08X\n", (unsigned int)reg.ALL);

    printf("Setting EN bit via bit-field...\n");
    reg.BIT.EN = 1U;
    printf("Register ALL after: 0x%08X\n", (unsigned int)reg.ALL);

    printf("Clearing register via ALL...\n");
    reg.ALL = 0U;
    printf("Register ALL final: 0x%08X\n", (unsigned int)reg.ALL);
}
/*
 * Discussion: Why CMSIS forbids bit-fields for hardware registers
 * ------------------------------------------------------------
 * The C standard does not define bit-field layout inside a
 * struct — bit order and padding are implementation-defined.
 * The same struct can map to different physical bits on
 * different compilers/targets, which is unacceptable for a
 * register that must be bit-exact.
 *
 * Two compiler-dependent behaviors break portability:
 * 1. Endianness/bit order - whether the first declared field
 *    sits at the LSB or MSB is not standardized; it varies by
 *    compiler and target.
 * 2. Padding/storage boundaries - compilers may insert padding
 *    or split fields across storage units differently, changing
 *    struct size and field positions across toolchains.
 *
 * Because of this, CMSIS defines registers as plain uint32_t
 * and uses mask/shift macros (REG |= (1UL << POS)) instead of
 * bit-fields, guaranteeing identical bit layout on every
 * compiler (GCC, ARMCC, IAR, etc.).
 */