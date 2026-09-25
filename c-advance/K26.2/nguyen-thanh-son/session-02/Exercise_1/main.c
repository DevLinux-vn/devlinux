#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint32_t global_const = 100U;
uint32_t global_init = 42U;
uint32_t global_uninit;

void print_memory_map(void);

int main(void)
{
    print_memory_map();
    return 0;
}

void print_memory_map(void)
{
    void (*p_text_fn)(void) = (void (*)(void))&main;
    uintptr_t addr_text;
    (void)memcpy(&addr_text, &p_text_fn, sizeof(addr_text));

    uintptr_t addr_rodata = (uintptr_t)&global_const;
    uintptr_t addr_data   = (uintptr_t)&global_init;
    uintptr_t addr_bss    = (uintptr_t)&global_uninit;

    uint32_t *p_heap_var = malloc(sizeof(*p_heap_var));
    if (p_heap_var == NULL)
    {
        (void)fprintf(stderr, "Fatal: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    *p_heap_var = 55U;
    uintptr_t addr_heap = (uintptr_t)p_heap_var;

    uint32_t stack_var = 7U;
    uintptr_t addr_stack = (uintptr_t)&stack_var;

    printf("=== Memory Segment Map ===\n");
    printf("[TEXT]   Address of main():            %p\n", (void *)addr_text);
    printf("[RODATA] Address of global_const:      %p\n", (void *)addr_rodata);
    printf("[DATA]   Address of global_init:       %p\n", (void *)addr_data);
    printf("[BSS]    Address of global_uninit:     %p\n", (void *)addr_bss);
    printf("[HEAP]   Address of heap_var:          %p\n", (void *)addr_heap);
    printf("[STACK]  Address of stack_var:         %p\n", (void *)addr_stack);

    printf("\n=== Segment Distances ===\n");
    printf("RODATA - TEXT:   %" PRIuPTR " bytes\n", addr_rodata - addr_text);
    printf("DATA   - RODATA: %" PRIuPTR " bytes\n", addr_data - addr_rodata);
    printf("BSS    - DATA:   %" PRIuPTR " bytes\n", addr_bss - addr_data);
    printf("HEAP   - BSS:    %" PRIuPTR " bytes\n", addr_heap - addr_bss);
    printf("STACK  - HEAP:   %" PRIuPTR " bytes\n", addr_stack - addr_heap);

    free(p_heap_var);
    p_heap_var = NULL; 
}


