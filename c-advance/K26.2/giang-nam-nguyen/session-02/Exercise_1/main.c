
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

// Function prototype.
void print_memory_map(void);

const uint32_t global_const = 100; //.rodata section.
uint32_t global_init        = 42;  // .data section.
uint32_t global_uninit;            // .bss section.


void print_memory_map(void)
{
    uint32_t stack_var   = 10; // Stack section.
    uint32_t *p_heap_var = (uint32_t*) malloc(sizeof(uint32_t)); // Heap section.
    if (NULL == p_heap_var)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    *p_heap_var = 20; // Initialize the heap variable.

    uintptr_t addr_text  = (uintptr_t)&print_memory_map; // Address of the function (text section).
    uintptr_t addr_rodta = (uintptr_t)&global_const;     // Address of the global constant (rodata section).
    uintptr_t addr_data  = (uintptr_t)&global_init;      // Address of the initialized global variable (data section).
    uintptr_t addr_bss   = (uintptr_t)&global_uninit;    // Address of the uninitialized global variable (bss section).
    uintptr_t addr_stack = (uintptr_t)&stack_var;        // Address of the local variable (stack section).
    uintptr_t addr_heap  = (uintptr_t)p_heap_var;        // Address of the heap variable (heap section).

    /* 
     * Print the memory map of the program.
     * When print the memory map, we can use the %p format specifier to print the addresses in a platform-independent way. 
     * Must cast the addresses to (void*) to avoid warnings. 
     */
    printf("=== Memory Segment Map ===\n");
    printf("[TEXT]    Address of main():        %p\n", (void*)addr_text);
    printf("[RODATA]  Address of global_const:  %p\n", (void*)addr_rodta);
    printf("[DATA]    Address of global_init:   %p\n", (void*)addr_data);
    printf("[BSS]     Address of global_uninit: %p\n", (void*)addr_bss);
    printf("[STACK]   Address of stack_var:     %p\n", (void*)addr_stack);
    printf("[HEAP]    Address of p_heap_var:    %p\n", (void*)addr_heap);

    /*
     * Calculate the distances between the memory segments.
     * "%" PRIuPTR " is used to print uintptr_t values in a platform-independent way.
     */
    printf("\n=== Segment Distances ===\n");
    printf("RODATA - TEXT: %" PRIuPTR " bytes\n", addr_rodta - addr_text);
    printf("DATA - RODATA: %" PRIuPTR " bytes\n", addr_data - addr_rodta);
    printf("BSS - DATA:    %" PRIuPTR " bytes\n", addr_bss - addr_data);
    printf("HEAP - BSS:    %" PRIuPTR " bytes\n", addr_heap - addr_bss);
    printf("STACK - HEAP:  %" PRIuPTR " bytes\n", addr_stack - addr_heap);

    free(p_heap_var);   // Free the allocated memory to avoid memory leak.
    p_heap_var = NULL;  // Set pointer to NULL after freeing.
}


int main(void)
{
    print_memory_map();
    return 0;
}