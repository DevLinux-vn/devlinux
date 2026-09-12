#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

typedef union {
    uint32_t val;
    uint8_t arr[5];
} unpacked_union_t;

typedef union __attribute__((packed)) {
    uint32_t val;
    uint8_t arr[5];
} packed_union_t;

typedef struct __attribute__((packed)) {
    unpacked_union_t u;
} packed_struct_with_unpacked_union_t;

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
    uint32_t     ALL;
    hw_reg_bits_t BIT;
} hw_reg_t;


int main(void)
{
    printf("=== Advanced Nested Packing ===\n");

    printf("Size of struct with UNPACKED union: %zu bytes (tail padding exists)\n", sizeof(packed_struct_with_unpacked_union_t));
    printf("Size of struct with PACKED union: %zu bytes (tail padding eliminated)\n", sizeof(packed_struct_with_packed_union_t));

    printf("\n");

    printf("=== Struct Bit-Fields & Hardware Mapping ===\n");
    printf("Size of hw_reg_bits_t: %zu bytes\n", sizeof(hw_reg_bits_t));

    hw_reg_t reg = {0};

    printf("Register ALL before: 0x%08" PRIx32 "\n", reg.ALL);

    printf("Setting EN bit via bit-field...\n");

    reg.BIT.EN = 1;

    printf("Register ALL after: 0x%08" PRIx32 "\n", reg.ALL);

    printf("Clearing register via ALL...\n");
    reg.ALL = 0;

    printf("Register ALL final: 0x%08" PRIx32 "\n", reg.ALL);

    return 0;
}