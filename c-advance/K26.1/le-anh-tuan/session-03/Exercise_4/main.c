/**
 * @file main.c
 * @brief Demonstrates nested packed unions and a peripheral register union pattern.
 *
 * Packed union explanation:
 * A union has the size of its largest member, but that size is also rounded up to
 * satisfy the union alignment requirement. In this exercise, the largest member is
 * the 5-byte array, while the uint32_t member gives the union a 4-byte alignment.
 * Therefore, the unpacked union becomes 8 bytes instead of 5 bytes because the
 * compiler adds tail padding. Packing only the outer struct removes padding around
 * the member, but it does not change the already padded size of the union member.
 * Packing the union itself lowers its alignment requirement and allows the union
 * size to become exactly 5 bytes.
 *
 * Hardware bit-field portability discussion:
 * The peripheral union pattern below is common in teaching examples, but strict
 * CMSIS-style register definitions avoid struct bit-fields for hardware mapping.
 * CMSIS uses explicit masks, shifts, and volatile integer registers instead.
 *
 * The main reason is portability. The C standard does not fully define how
 * bit-fields are laid out inside a storage unit. Two compiler-dependent behaviors
 * can break register mapping:
 *
 * 1. Bit numbering and endianness interpretation:
 *    A declaration such as EN : 1 does not portably guarantee that EN maps to
 *    hardware bit 0 on every compiler, target, and ABI. Different implementations
 *    may allocate bit-fields from the least-significant bit side or from the
 *    most-significant bit side of the storage unit.
 *
 * 2. Padding and allocation boundaries:
 *    A compiler may insert padding, choose different allocation units, or cross
 *    storage-unit boundaries differently. This can make the bit-field struct size
 *    or field position differ from the real hardware register layout.
 *
 * For those reasons, this file intentionally demonstrates the pattern, but a
 * production CMSIS peripheral header should prefer uint32_t register members with
 * named mask and shift macros. This exercise also intentionally uses unions, which
 * is a documented deviation from MISRA-C:2012 Rule 19.2.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Number of bytes in the demonstration array member.
 */
#define UNION_BYTE_ARRAY_SIZE (5U)

/**
 * @brief Unpacked union containing a 32-bit value and a 5-byte array.
 */
typedef union
{
    uint32_t value;
    uint8_t bytes[UNION_BYTE_ARRAY_SIZE];
} unpacked_union_t;

/**
 * @brief Packed struct containing an unpacked union.
 */
typedef struct __attribute__((packed))
{
    unpacked_union_t payload;
} packed_struct_with_unpacked_union_t;

/**
 * @brief Packed union containing a 32-bit value and a 5-byte array.
 */
typedef union __attribute__((packed))
{
    uint32_t value;
    uint8_t bytes[UNION_BYTE_ARRAY_SIZE];
} packed_union_t;

/**
 * @brief Packed struct containing a packed union.
 */
typedef struct __attribute__((packed))
{
    packed_union_t payload;
} packed_struct_with_packed_union_t;

/**
 * @brief Hypothetical 32-bit hardware register bit layout.
 */
typedef struct
{
    uint32_t enable   : 1;
    uint32_t mode     : 3;
    uint32_t flag     : 1;
    uint32_t reserved : 27;
} hw_reg_bits_t;

/**
 * @brief Peripheral register view with full-word and bit-field access.
 */
typedef union
{
    uint32_t ALL;
    hw_reg_bits_t BIT;
} hw_reg_t;


/**
 * @brief Touches packed and unpacked union members so static analyzers know
 *        the members are intentionally used.
 */
static void demonstrate_union_members(void)
{
    unpacked_union_t unpacked_data = {UINT32_C(0x11223344)};
    packed_union_t packed_data = {UINT32_C(0xAABBCCDD)};

    printf("Unpacked union demo: value=0x%08" PRIX32 ", byte[0]=0x%02" PRIX8 "\n",
           unpacked_data.value,
           unpacked_data.bytes[0]);

    printf("Packed union demo: value=0x%08" PRIX32 ", byte[0]=0x%02" PRIX8 "\n\n",
           packed_data.value,
           packed_data.bytes[0]);
}


/**
 * @brief Program entry point.
 *
 * @return 0 on success.
 */
int main(void)
{
    hw_reg_t register_value = {0U};

    printf("=== Advanced Nested Packing ===\n");
    printf("Size of struct with UNPACKED union: %zu bytes (tail padding exists)\n",
           sizeof(packed_struct_with_unpacked_union_t));
    printf("Size of struct with PACKED union: %zu bytes (tail padding eliminated)\n\n",
           sizeof(packed_struct_with_packed_union_t));
    
    
    demonstrate_union_members();

    printf("=== Struct Bit-Fields & Hardware Mapping ===\n");
    printf("Size of hw_reg_bits_t: %zu bytes\n", sizeof(hw_reg_bits_t));

    printf("Register ALL before: 0x%08" PRIX32 "\n", register_value.ALL);

    printf("Setting EN bit via bit-field...\n");
    register_value.BIT.enable = 1U;
    printf("Register ALL after: 0x%08" PRIX32 "\n", register_value.ALL);

    printf("Clearing register via ALL...\n");
    register_value.ALL = 0U;
    printf("Register ALL final: 0x%08" PRIX32 "\n", register_value.ALL);

    return 0;
}