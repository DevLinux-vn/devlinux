#include "hw_union.h"

void peripheral_union(hw_reg_t *p_reg)
{
    printf("\n=== Struct Bit-Fields & Hardware Mapping ===\n");

    printf("Size of hw_reg_bits_t: %zu bytes\n", sizeof(p_reg->BIT));
    printf("Register ALL before: 0x%08x\n", p_reg->ALL);

    printf("Setting EN bit via bit-field...\n");
    p_reg->BIT.EN = 1U;
    printf("Register ALL after: 0x%08x\n", p_reg->ALL); 

    printf("Clearing register via ALL...\n");
    p_reg->ALL = 0U;
    printf("Register ALL final: 0x%08x\n", p_reg->ALL);

}

