#include "struct_padding_analyzer.h"

void print_unoptimized_info(void)
{
    printf("[Unoptimized Struct]\n");
    printf("Size: %zu bytes\n", sizeof(unoptimized_t));
    printf("Offsets: c(%zu), i(%zu), d(%zu), s(%zu)\n",
       offsetof(unoptimized_t, c),
       offsetof(unoptimized_t, i),
       offsetof(unoptimized_t, d),
       offsetof(unoptimized_t, s));
    printf("\n");
}

void print_optimized_info(void)
{
    printf("[Optimized Struct]\n");
    printf("Size: %zu bytes\n", sizeof(optimized_t));
    printf("Offsets: d(%zu), i(%zu), s(%zu), c(%zu)\n",
       offsetof(optimized_t, d),
       offsetof(optimized_t, i),
       offsetof(optimized_t, s),
       offsetof(optimized_t, c));
    printf("\n");
}

void print_packed_info(void)
{
    printf("[Packed Struct]\n");
    printf("Size: %zu bytes\n", sizeof(packed_t));
    printf("Offsets: c(%zu), i(%zu), d(%zu), s(%zu)\n",
       offsetof(packed_t, c),
       offsetof(packed_t, i),
       offsetof(packed_t, d),
       offsetof(packed_t, s));
    printf("\n");
}

void demonstrate_packed_access(void)
{
    packed_t data;
    data.i = 10;
    
    // Warning: unaligned pointer
    // int *p_data = &data.i; 
    // *p_data = 20;

    if (data.i == 10)
    {
        printf("Attempting direct access to packed member... Success!\n");
    }
}