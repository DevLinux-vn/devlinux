#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "memory_map.h"

/* Rodata */
const uint32_t global_const = 100;

/* data */
uint32_t global_init = 42;

/* bss */

uint32_t global_uninit;

void print_memory_map(void) 
{
    uint32_t stack_var = 10;

    uint32_t *p_heap_var = malloc(sizeof(uint32_t));
    if(p_heap_var == NULL)
    {
        return;
    }

    uintptr_t text = (uintptr_t)&print_memory_map;
    uintptr_t rodata = (uintptr_t)&global_const;
    uintptr_t data = (uintptr_t)&global_init;
    uintptr_t bss = (uintptr_t)&global_uninit;
    uintptr_t heap = (uintptr_t)p_heap_var;
    uintptr_t stack = (uintptr_t)&stack_var;

    printf("=== Memory Segment Map ===\n");

    printf("[TEXT]   Address of main():            %p\n", (void *)(uintptr_t)print_memory_map);
    printf("[RODATA] Address of global_const:      %p\n", (void*)&global_const);
    printf("[DATA]   Address of global_init:       %p\n", (void*)&global_init);
    printf("[BSS]    Address of global_uninit:     %p\n", (void*)&global_uninit);
    printf("[HEAP]   Address of heap_var:          %p\n", (void*)p_heap_var);
    printf("[STACK]  Address of stack_var:         %p\n", (void*)&stack_var);

    printf("\n");

    printf("=== Segment Distances ===\n");

    printf("RODATA - TEXT:   %lu bytes\n", (unsigned long)(rodata - text));
    printf("DATA   - RODATA: %lu bytes\n", (unsigned long)(data - rodata));
    printf("BSS    - DATA:   %lu bytes\n", (unsigned long)(bss - data));
    printf("HEAP   - BSS:    %lu bytes\n", (unsigned long)(heap - bss));
    printf("STACK  - HEAP:   %lu bytes\n", (unsigned long)(stack - heap));

    free(p_heap_var);
    p_heap_var = NULL;

}
