#include "packed_union.h"

void union_size(void)
{
    printf("=== Advanced Nested Packing ===\n");
    printf("Size of struct with UNPACKED union: %zu bytes (tail padding exists)\n", sizeof(packed_struct_with_unpacked_union_t));
    printf("Size of struct with PACKED union: %zu bytes (tail padding eliminated)\n", sizeof(packed_struct_with_packed_union_t));
}