#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void);

const uint32_t GLOBAL_VARIABLE = 100;
uint32_t global_initialized = 50;
uint32_t global_uninitialized;

/**
 * @brief Print memory map of all 6 segments (text, rodata, data, bss, heap, stack)
 *        and the distance in bytes between consecutive segments (TEXT -> RODATA ->
 *        DATA -> BSS -> HEAP -> STACK).
 * @param No parameter
 * @return 0 on success, -1 if malloc fails
 */
uint8_t print_memory_map(void)
{
    int local_init = 20;

    int *p_ptr = (int *)malloc(10 * sizeof(int));
    if (p_ptr == NULL)
    {
        fprintf(stderr, "Error: malloc failed in print_memory_map()\n");
        return -1;
    }

    uintptr_t addr_text  = (uintptr_t)&main;
    uintptr_t addr_rodata = (uintptr_t)&GLOBAL_VARIABLE;
    uintptr_t addr_data   = (uintptr_t)&global_initialized;
    uintptr_t addr_bss    = (uintptr_t)&global_uninitialized;
    uintptr_t addr_heap   = (uintptr_t)p_ptr;
    uintptr_t addr_stack  = (uintptr_t)&local_init;

    printf("[TEXT]   Address of main():            %p\n", (void *)addr_text);
    printf("[RODATA] Address of global_variable:   %p\n", (void *)addr_rodata);
    printf("[DATA]   Address of global_initialized:       %p\n", (void *)addr_data);
    printf("[BSS]    Address of global_uninitialized:     %p\n", (void *)addr_bss);
    printf("[HEAP]   Address of p_ptr:              %p\n", (void *)addr_heap);
    printf("[STACK]  Address of local_init:         %p\n", (void *)addr_stack);

    intptr_t delta_rodata_text = (intptr_t)addr_rodata - (intptr_t)addr_text;
    intptr_t delta_data_rodata = (intptr_t)addr_data   - (intptr_t)addr_rodata;
    intptr_t delta_bss_data    = (intptr_t)addr_bss    - (intptr_t)addr_data;
    intptr_t delta_heap_bss    = (intptr_t)addr_heap   - (intptr_t)addr_bss;
    intptr_t delta_stack_heap  = (intptr_t)addr_stack  - (intptr_t)addr_heap;

    printf("RODATA - TEXT   = %jd byte\n", (intmax_t)delta_rodata_text);
    printf("DATA   - RODATA = %jd byte\n", (intmax_t)delta_data_rodata);
    printf("BSS    - DATA   = %jd byte\n", (intmax_t)delta_bss_data);
    printf("HEAP   - BSS    = %jd byte\n", (intmax_t)delta_heap_bss);
    printf("STACK  - HEAP   = %jd byte\n", (intmax_t)delta_stack_heap);

    free(p_ptr);
    p_ptr = NULL;

    return 0;
}

/**
 * @brief Entry point. Calls print_memory_map() to dump the 6 memory segments
 *        and the byte distance between each consecutive pair.
 * @return 0 on success, -1 if print_memory_map() fails
 */
int main(void)
{
    uint8_t result = print_memory_map();
    if (result != 0)
    {
        fprintf(stderr, "Error: print_memory_map() failed\n");
        return -1;
    }

    return 0;
}

/*Size output
   text	   data	    bss	    dec	    hex	filename
   3013	    636	     16	   3665	    e51	./main

    0000000000004010 D global_initialized
    000000000000402c B global_uninitialized

*/