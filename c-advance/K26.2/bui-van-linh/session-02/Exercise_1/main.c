#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const uint32_t global_variable = 100;
uint32_t global_data = 50;
uint32_t global_uninit;

/**
 * @brief Print memory map of all 6 segments (text, rodata, data, bss, heap, stack)
 *        and the distance in bytes between consecutive segments.
 * @param None
 * @return 0 on success, -1 if malloc fails
 */
uint8_t print_memory_map(void)
{
    /* Address of RODATA computed locally (no need for global scope) */
    uintptr_t addr_global_variable = (uintptr_t)&global_variable;

    int local_init = 20;

    int *p_ptr = (int *)malloc(10 * sizeof(int));
    if (p_ptr == NULL)
    {
        fprintf(stderr, "Error: malloc failed in print_memory_map()\n");
        return -1;
    }

    printf("TEXT  address of print_memory_map: %p\n", (void *)&print_memory_map);
    printf("RODATA address of CONST GLOBAL VARIABLE: %p\n", (void *)&global_variable);
    printf("DATA  address of INITIALIZED GLOBAL VARIABLE: %p\n", (void *)&global_data);
    printf("BSS   address of UNINITIALIZED GLOBAL VARIABLE: %p\n", (void *)&global_uninit);
    printf("HEAP  address of p_ptr: %p\n", (void *)p_ptr);
    printf("STACK address of LOCAL VARIABLE: %p\n", (void *)&local_init);

    uintptr_t addr_global_data   = (uintptr_t)&global_data;
    uintptr_t addr_global_uninit = (uintptr_t)&global_uninit;
    uintptr_t addr_ptr           = (uintptr_t)p_ptr;
    uintptr_t addr_local_init    = (uintptr_t)&local_init;

    uintptr_t delta_2 = addr_global_data - addr_global_variable;
    printf("DATA - TEXT = %ju byte\n", (uintmax_t)delta_2);

    uintptr_t delta_3 = addr_global_uninit - addr_global_data;
    printf("BSS - DATA = %ju byte\n", (uintmax_t)delta_3);

    uintptr_t delta_4 = addr_ptr - addr_global_uninit;
    printf("HEAP - BSS = %ju byte\n", (uintmax_t)delta_4);

    uintptr_t delta_5 = addr_local_init - addr_ptr;
    printf("STACK - HEAP = %ju byte\n", (uintmax_t)delta_5);

    free(p_ptr);
    p_ptr = NULL;

    return 0;
}

/**
 * @brief Entry point. Prints the distance between RODATA and TEXT segments,
 *        then calls print_memory_map() and checks its return value.
 * @return 0 on success, -1 if print_memory_map() fails
 */
int main(void)
{
    uintptr_t addr_main = (uintptr_t)&main;
    uintptr_t addr_global_variable = (uintptr_t)&global_variable;

    /* Use intptr_t here since this delta can be negative depending on layout */
    intptr_t delta_1 = (intptr_t)addr_global_variable - (intptr_t)addr_main;
    printf("RODATA - TEXT = %jd byte\n", (intmax_t)delta_1);

    uint8_t result = print_memory_map();
    if (result != 0)
    {
        fprintf(stderr, "Error: print_memory_map() failed\n");
        return -1;
    }

    return 0;
}