#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

    uint32_t g_var_init = 42;
    const uint32_t g_var_const = 100;
    uint32_t g_var_uninit;

int print_memory_map(void) {
    int (*p_function)(void) = &print_memory_map;
    uint8_t* p_heap_segment = (uint8_t*) malloc (sizeof(uint8_t) * 2);
    uint8_t stack_segment = 10;

    printf("=== Memory Segment Map ===\n");

    if (p_heap_segment == NULL) {
        printf ("Failed to allocate p_heap\n");
        return 1;
    } else if (p_heap_segment != NULL) {
        printf("[HEAP]   Address of heap_var: %p\n", (void*) p_heap_segment);
    }

    if (p_function == NULL) {
        printf ("p_function failed to point\n");
        return 1;
    } else if (p_function != NULL) {
        printf("[TEXT] Address of main(): %p\n", (void*) ((uintptr_t)p_function));
    }

    printf("[RODATA] Address of global_const: %p\n", (void*) &g_var_const);
    printf("[DATA]   Address of global_init: %p\n", (void*) &g_var_init);
    printf("[BSS]    Address of global_uninit:: %p\n",(void*) &g_var_uninit);
    printf("[STACK]  Address of stack_var:  %p\n",(void*) &stack_segment);

    free(p_heap_segment);

    p_heap_segment = NULL;

    uintptr_t u_text   = (uintptr_t)p_function;
    uintptr_t u_rodata = (uintptr_t)&g_var_const;
    uintptr_t u_data   = (uintptr_t)&g_var_init;
    uintptr_t u_bss    = (uintptr_t)&g_var_uninit;
    uintptr_t u_heap   = (uintptr_t)p_heap_segment;
    uintptr_t u_stack  = (uintptr_t)&stack_segment;

    uint64_t rodata_text_dis = (uint64_t) (u_rodata > u_text ? u_rodata - u_text : u_text - u_rodata);
    uint64_t data_rodata_dis = (uint64_t) (u_data > u_rodata ? u_data - u_rodata : u_rodata - u_data);
    uint64_t bss_data_dis = (uint64_t) (u_bss > u_data ? u_bss - u_data : u_data - u_bss);
    uint64_t heap_bss_dis = (uint64_t) (u_heap > u_bss ? u_heap - u_bss : u_bss - u_heap);
    uint64_t stack_heap_dis = (uint64_t) (u_stack > u_heap ? u_stack - u_heap : u_heap - u_stack);

    printf("=== Segment Distances ===\n");
    printf("RODATA - TEXT: %ld bytes\n", rodata_text_dis);
    printf("DATA   - RODATA: %ld bytes\n", data_rodata_dis);
    printf("BSS    - DATA:   %ld bytes\n", bss_data_dis);
    printf("HEAP   - BSS:    %ld bytes\n", heap_bss_dis);
    printf("STACK  - HEAP:   %ld bytes\n", stack_heap_dis);

    return 0;
}

int main() {
    print_memory_map();

    return 0;
}