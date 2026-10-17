#include "packed_union.h"
#include "hw_union.h"

/**
1. Explain why packing the union itself was necessary to eliminate tail padding.
    -Packing only the struct was not enough because the union itself still had its natural 
    alignment requirement. Although the packed struct removed padding between its members, the unpacked 
    union still contained tail padding so that its size became a multiple of its alignment. As a result, 
    the struct still included the union's padded size. By applying __attribute__((packed)) directly to the union, 
    its alignment requirement was reduced to 1 byte, eliminating the tail padding. Therefore, the packed struct 
    used the union's actual data size (5 bytes) instead of its padded size. 
2. Why does the strict CMSIS standard forbid using struct bit-fields for mapping hardware registers?
    - The CMSIS standard forbids using struct bit-fields to map hardware registers because the C standard does 
    not define the layout of bit-fields. Their bit order, alignment, padding, and packing are implementation-defined, 
    meaning they can vary between compilers, compiler versions, target architectures, and compiler options. This can 
    cause the same source code to access the wrong register bits, leading to incorrect or unsafe hardware behavior. 
    Instead, CMSIS recommends using bit masks and bitwise operations, which provide predictable and portable access 
    to hardware registers.
3. What are the two compiler-dependent behaviors (endianness and padding boundaries) that break portability?
    - The two compiler-dependent behaviors that break portability are:
        - Endianness:
            - Endianness determines the byte order used to store multi-byte data in memory.
            - Different architectures use different byte orders (e.g., little-endian or big-endian).
            - Code that assumes a specific byte order may produce incorrect results on a system with 
            a different endianness.
        - Padding and Alignment Boundaries:
            - Compilers may insert padding bytes between or after structure members to satisfy alignment requirements.
            - The amount and location of padding depend on the compiler, target architecture, and compiler options.
            - As a result, the memory layout of a struct or union may differ across platforms, making direct memory mapping 
            or binary data exchange unreliable.

 */

int main()
{
    hw_reg_t *p_reg = malloc(sizeof(hw_reg_t));

    union_size();
    peripheral_union(p_reg);

    free(p_reg);
    return 0;
}