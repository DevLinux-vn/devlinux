#include "memory_map.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* RODATA */
const uint32_t global_const = 100U;

/* DATA */
uint32_t global_init = 42U;

/* BSS */
uint32_t global_uninit;

/**
 * @brief Print addresses of variables in each memory segment and calculate distances.
 * Demonstrates memory layout: text, rodata, data, bss, heap, stack.
 */
void print_memory_map(void)
{
    /* STACK */
    uint32_t stack_var = 10U;

    /* HEAP */
    uint32_t *p_heap = malloc(sizeof(uint32_t));

    if (p_heap == NULL)
    {
        return;
    }

    /* PRINT_ADDRESS_VARIABLE */
    printf("=== Memory Segment Map ===\n");
    printf("[TEXT]   Address of print_memory_map():     %p\n", (void *)(uintptr_t)&print_memory_map);
    printf("[RODATA] Address of global_const:           %p\n", (void *)&global_const);
    printf("[DATA]   Address of global_init:            %p\n", (void *)&global_init);
    printf("[BSS]    Address of global_uninit:          %p\n", (void *)&global_uninit);
    printf("[HEAP]   Address of heap_var:               %p\n", (void *)p_heap);
    printf("[STACK]  Address of stack_var:              %p\n", (void *)&stack_var);

    /* CONVERT_ADDRESS */
    uintptr_t addr_print_memory_map = (uintptr_t)&print_memory_map;
    uintptr_t addr_global_const = (uintptr_t)&global_const;
    uintptr_t addr_global_init = (uintptr_t)&global_init;
    uintptr_t addr_global_uninit = (uintptr_t)&global_uninit;
    uintptr_t addr_stack_var = (uintptr_t)&stack_var;
    uintptr_t addr_p_heap = (uintptr_t)p_heap;

    /* PRINT_DISTANCE_SEGMENT */
    printf("=== Segment Distances ===\n");
    printf("RODATA - TEXT:      %lu bytes\n", (unsigned long)(addr_global_const - addr_print_memory_map));
    printf("DATA   - RODATA:    %lu bytes\n", (unsigned long)(addr_global_init - addr_global_const));
    printf("BSS    - DATA:      %lu bytes\n", (unsigned long)(addr_global_uninit - addr_global_init));
    printf("HEAP   - BSS:       %lu bytes\n", (unsigned long)(addr_p_heap - addr_global_uninit));
    printf("STACK  - HEAP:      %lu bytes\n", (unsigned long)(addr_stack_var - addr_p_heap));

    free(p_heap);
    p_heap = NULL;
}

