
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Application success return code.
 */
#define APP_SUCCESS (0)

/**
 * @brief Unpacked union containing a 32-bit value and a 5-byte array.
 */
typedef union
{
    uint32_t val;
    uint8_t arr[5U];
} unpacked_union_t;

/**
 * @brief Packed struct containing an unpacked union.
 */
typedef struct __attribute__((packed))
{
    unpacked_union_t u;
} packed_struct_with_unpacked_union_t;

/**
 * @brief Packed union containing a 32-bit value and a 5-byte array.
 */
typedef union __attribute__((packed))
{
    uint32_t val;
    uint8_t arr[5U];
} packed_union_t;

/**
 * @brief Packed struct containing a packed union.
 */
typedef struct __attribute__((packed))
{
    packed_union_t u;
} packed_struct_with_packed_union_t;

/**
 * @brief Hypothetical 32-bit hardware register bit layout.
 */
typedef struct
{
    uint32_t EN   : 1;
    uint32_t MODE : 3;
    uint32_t FLAG : 1;
    uint32_t res  : 27;
} hw_reg_bits_t;

/**
 * @brief Peripheral register union allowing full-word and bit-field access.
 */
typedef union
{
    uint32_t ALL;
    hw_reg_bits_t BIT;
} hw_reg_t;

/**
 * @brief Print nested union packing result.
 */
static void print_packing_result(void)
{
    (void)printf("=== Advanced Nested Packing ===\n");
    (void)printf("Size of struct with UNPACKED union: %zu bytes "
                 "(tail padding exists)\n",
                 sizeof(packed_struct_with_unpacked_union_t));

    (void)printf("Size of struct with PACKED union: %zu bytes "
                 "(tail padding eliminated)\n\n",
                 sizeof(packed_struct_with_packed_union_t));
}

/**
 * @brief Demonstrate bit-field and full-register union access.
 */
static void demonstrate_register_access(void)
{
    hw_reg_t reg = { 0U };

    (void)printf("=== Struct Bit-Fields & Hardware Mapping ===\n");
    (void)printf("Size of hw_reg_bits_t: %zu bytes\n", sizeof(hw_reg_bits_t));
    (void)printf("Register ALL before: 0x%08X\n", (unsigned int)reg.ALL);

    (void)printf("Setting EN bit via bit-field...\n");
    reg.BIT.EN = 1U;
    (void)printf("Register ALL after: 0x%08X\n", (unsigned int)reg.ALL);

    (void)printf("Clearing register via ALL...\n");
    reg.ALL = 0U;
    (void)printf("Register ALL final: 0x%08X\n", (unsigned int)reg.ALL);
}

/*Comment output
=== Advanced Nested Packing ===
Size of struct with UNPACKED union: 8 bytes (tail padding exists)
Size of struct with PACKED union: 5 bytes (tail padding eliminated)

=== Struct Bit-Fields & Hardware Mapping ===
Size of hw_reg_bits_t: 4 bytes
Register ALL before: 0x00000000
Setting EN bit via bit-field...
Register ALL after: 0x00000001
Clearing register via ALL...
Register ALL final: 0x00000000*/
/**
 * @brief Program entry point.
 *
 * @return APP_SUCCESS when completed successfully.
 */
int main(void)
{
    print_packing_result();
    demonstrate_register_access();

    return APP_SUCCESS;
}

