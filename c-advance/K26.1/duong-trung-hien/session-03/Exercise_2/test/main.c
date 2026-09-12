#include "struct_padding_analyzer.h"
/*
 * Trade-offs and dangers of unaligned memory access:
 * - Packed structs reduce memory usage by removing padding.
 * - However, unaligned access may reduce performance because the CPU
 *   may require multiple memory operations.
 * - On some ARM processors, unaligned pointer access can cause a
 *   HardFault exception.
 * - Direct member access is handled by the compiler, while taking the
 *   address of a packed member creates an unaligned pointer, making
 *   pointer access unsafe and potentially non-portable.
 */
int main()
{
    print_unoptimized_info();
    print_optimized_info();
    print_packed_info();
    demonstrate_packed_access();

    return 0;
}