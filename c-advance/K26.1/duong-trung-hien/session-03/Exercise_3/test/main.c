#include "system_permissions.h"

/**
1. Why might the size of this enum differ between a GCC compiler for an x86 PC vs an ARM Cortex-M micro-controller?
    - The size of an enum in C is not fixed by the C standard.
    - Typically: On an x86 PC using GCC: The compiler usually stores an enum as an int. Therefore: sizeof(enum) = 4 bytes
    - On an ARM Cortex-M (embedded system): The compiler may choose a smaller integer type to save memory.
    - For example: sizeof(enum) = 1 byte or 2 bytes
    - The actual size depends on the compiler and the compiler options (such as -fshort-enums).
2. What is the compiler flag -fshort-enums and why is it dangerous when linking libraries?
    - The -fshort-enums compiler flag tells the compiler to use the smallest integer type that can 
    represent all values of an enum, instead of using the default int.
    - If one library is compiled with -fshort-enums and another is compiled without it, the same enum 
    type may have different sizes (for example, 1 byte vs. 4 bytes). This can cause an ABI (Application Binary Interface) 
    mismatch, leading to incorrect data being passed between functions, memory corruption, or undefined behavior.
 */

int main()
{
    uint8_t user_perms = 0x03U;
    printf("=== Bitmask Permissions Tester ===\n");
    printf("Enum size: %zu bytes (Standard GCC)\n", sizeof(sys_perms_e));
    printf("User 1 (Read|Write): 0x%02x\n", user_perms);

    result_check(user_perms, PERM_READ);
    result_check(user_perms, PERM_EXECUTE);
    result_check(user_perms, PERM_READ | PERM_WRITE);
    result_check(user_perms, PERM_EXECUTE | PERM_READ);

    return 0;
}