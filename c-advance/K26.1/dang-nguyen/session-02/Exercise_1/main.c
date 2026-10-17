#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#define APP_SUCCESS (0)
#define APP_FAILURE (-1)

#define ARG_NUM     (2)

int32_t (*p_main_func)(void);
const uint32_t global_const = 100U;
uint32_t global_init = 42;
uint32_t global_uninit;

/**
 * @brief Print memory layout map for all segments (text, rodata, data, bss, heap, stack).
 * 
 * Declares variables in each memory segment, prints their addresses, and calculates
 * distances between segments using pointer arithmetic.
 */
void print_memory_map(void);

void print_memory_map(void)
{
    uint32_t *p_ptr = (uint32_t *)malloc(sizeof(uint32_t));

    if (NULL != p_ptr)
    {
        uint32_t my_var = 1U;
        uintptr_t text_addr = (uintptr_t)p_main_func;
        uintptr_t rodata_addr = (uintptr_t)&global_const;
        uintptr_t data_addr = (uintptr_t)&global_init;
        uintptr_t bss_addr = (uintptr_t)&global_uninit;
        uintptr_t heap_addr = (uintptr_t)p_ptr;
        uintptr_t stack_addr = (uintptr_t)&my_var;
    
        printf("=== Memory Segment Map ===\n");
        printf("[TEXT]   Address of main():            %p\n", (void *)text_addr);
        printf("[RODATA] Address of global_const:      %p\n", (void *)rodata_addr);
        printf("[DATA]   Address of global_init:       %p\n", (void *)data_addr);
        printf("[BSS]    Address of global_uninit:     %p\n", (void *)bss_addr);
        printf("[HEAP]   Address of heap_var:          %p\n", (void *)heap_addr);
        printf("[STACK]  Address of stack_var:         %p\n", (void *)stack_addr);

        printf("\n=== Segment Distances ===\n");
        printf("RODATA - TEXT:   %" PRIu64 " bytes\n", (uint64_t)((rodata_addr > text_addr) ? (rodata_addr - text_addr) : (text_addr - rodata_addr)));
        printf("DATA   - RODATA: %" PRIu64 " bytes\n", (uint64_t)((data_addr > rodata_addr) ? (data_addr - rodata_addr) : (rodata_addr - data_addr)));
        printf("BSS    - DATA:   %" PRIu64 " bytes\n", (uint64_t)((bss_addr > data_addr) ? (bss_addr - data_addr) : (data_addr - bss_addr)));
        printf("HEAP   - BSS:    %" PRIu64 " bytes\n", (uint64_t)((heap_addr > bss_addr) ? (heap_addr - bss_addr) : (bss_addr - heap_addr)));
        printf("STACK  - HEAP:   %" PRIu64 " bytes\n", (uint64_t)((stack_addr > heap_addr) ? (stack_addr - heap_addr) : (heap_addr - stack_addr)));
    
        free(p_ptr);
        p_ptr = NULL;
    }
    else
    {
        (void)p_ptr;
        fprintf(stderr, "malloc() failed\n");
    }
}

int32_t main(void)
{
    p_main_func = &main;

    print_memory_map();

    return 0;
}

/*
 * Verification Output
 * 
 * $ size mem_layout
 * text    data     bss     dec     hex filename
 * 2871     636      32    3539     dd3 mem_layout
 * 
 * $ nm mem_layout | grep -i global_init
 * 0000000000004010 D global_init
 * 
 * $ nm mem_layout | grep -i global_uninit
 * 0000000000004038 B global_uninit
 */