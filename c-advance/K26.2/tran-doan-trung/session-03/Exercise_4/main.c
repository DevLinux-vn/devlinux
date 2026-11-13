/**
 * @file main.c
 * @brief Exercise 4 - Packed Unions, Bit-fields & Peripheral Union Pattern
 */

#include <stdint.h>
#include <stdio.h>

// --- Task 1: Unions ---
typedef union {
    uint32_t val;
    uint8_t  arr[5];
} unpacked_union_t;

typedef struct __attribute__((packed)) {
    unpacked_union_t u;
} packed_struct_with_unpacked_union_t;

// --- Task 2 & 3: Peripheral Union Pattern ---
typedef union __attribute__((packed)) {
    uint32_t val;
    uint8_t  arr[5];
} packed_union_t;

/**
 * @brief Struct packed chứa union đã pack -> tổng kích thước giảm còn 5 bytes.
 */
typedef struct __attribute__((packed)) {
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

int main(void) {
    packed_struct_with_unpacked_union_t s_unpacked = { .u = { .arr = { 0U } } };
    packed_struct_with_packed_union_t   s_packed   = { .u = { .arr = { 0U } } };
    (void)s_unpacked.u.val;
    (void)s_unpacked.u.arr[0];
    (void)s_packed.u.val;
    (void)s_packed.u.arr[0];

    printf("=== Advanced Nested Packing ===\n");
    printf("Size of struct with UNPACKED union: %zu bytes (tail padding exists)\n",
           sizeof(packed_struct_with_unpacked_union_t));
    
    printf("Size of struct with PACKED union: %zu bytes (tail padding eliminated)\n",
           sizeof(packed_struct_with_packed_union_t));

    printf("\n=== Struct Bit-Fields & Hardware Mapping ===\n");
    printf("Size of hw_reg_bits_t: %zu bytes\n", sizeof(hw_reg_bits_t));

    hw_reg_t hw_reg = { .ALL = 0x00000000U };
    printf("Register ALL before: 0x%08X\n", hw_reg.ALL);

    printf("Setting EN bit via bit-field...\n");
    hw_reg.BIT.EN = 1U;

    printf("Register ALL after: 0x%08X\n", hw_reg.ALL);
    printf("Clearing register via ALL...\n");

    hw_reg.ALL = 0x00000000U;
    printf("Register ALL final: 0x%08X\n", hw_reg.ALL);

    return 0;
}

/*
* DISCUSSION TASK NUMBER 1:
 * Q: Why is packing the union itself necessary to eliminate tail padding?
 * A: Applying __attribute__((packed)) to the outer struct only removes padding between struct members, not inside the nested union. Because the union contains a uint32_t (4-byte alignment), its own size is padded to 8 bytes (5 bytes array + 3 bytes tail padding). Packing the union itself reduces its alignment requirement to 1 byte, shrinking the size to 5 bytes.

 * DISCUSSION TASK NUMBER 4:
 Q: Why does the strict CMSIS standard forbid using struct bit-fields for mapping hardware registers?
 A: The strict CMSIS standard forbids using struct bit-fields for mapping hardware registers because the layout of bit-fields is implementation-defined and can vary between different compilers and architectures. This can lead to inconsistencies in how the bits are packed and accessed, potentially causing incorrect behavior when interacting with hardware registers. Instead, CMSIS recommends using unions with explicit bit manipulation to ensure predictable and portable access to hardware registers.
 
 Q:What are the two compiler-dependent behaviors (endianness and padding boundaries) that break portability?
 A: The two compiler-dependent behaviors that break portability are:
   - Endianness: The order in which bytes are stored in memory can differ between big-endian and little-endian systems. This affects how multi-byte values are interpreted when accessed through bit-fields, leading to potential misinterpretation of the data.
   - Padding Boundaries: Compilers may insert padding between struct members to align them according to the architecture's requirements. This can result in unexpected gaps in memory layout, causing issues when accessing hardware registers or when the struct is used for communication between different systems.
 */