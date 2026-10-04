/**
 * @file main.c
 * @brief Session 02 - Exercise 1: Memory Segment Analyzer
 *
 * In dia chi cua bien o tung vung nho (text, rodata, data, bss, heap, stack)
 * va tinh khoang cach giua cac vung.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

void print_memory_map(void);

/* bien toan cuc cho tung vung */
const uint32_t global_const = 100;  /* rodata */
uint32_t global_init = 42;          /* data */
uint32_t global_uninit;             /* bss */

/**
 * @brief In dia chi cac vung nho va khoang cach giua chung.
 */
void print_memory_map(void)
{
    uint32_t stack_var = 5;   /* stack - phai khoi tao truoc (MISRA 9.1) */
    uint32_t *p_heap_var = NULL;

    /* heap */
    p_heap_var = malloc(sizeof(uint32_t));
    if (p_heap_var == NULL)
    {
        printf("malloc failed\n");
        exit(1);
    }
    *p_heap_var = 10;

    /* doi dia chi sang so de tinh khoang cach (dung uintptr_t) */
    /* ham main khong ep thang sang void* duoc vi -pedantic bao loi,
       nen phai ep qua uintptr_t truoc */
    uintptr_t text_addr = (uintptr_t)&print_memory_map;
    uintptr_t rodata_addr = (uintptr_t)&global_const;
    uintptr_t data_addr = (uintptr_t)&global_init;
    uintptr_t bss_addr = (uintptr_t)&global_uninit;
    uintptr_t heap_addr = (uintptr_t)p_heap_var;
    uintptr_t stack_addr = (uintptr_t)&stack_var;

    printf("=== Memory Segment Map ===\n");
    printf("[TEXT]   Address of print_memory_map(): %p\n", (void *)text_addr);
    printf("[RODATA] Address of global_const:       %p\n", (const void *)&global_const);
    printf("[DATA]   Address of global_init:        %p\n", (void *)&global_init);
    printf("[BSS]    Address of global_uninit:      %p\n", (void *)&global_uninit);
    printf("[HEAP]   Address of heap_var:           %p\n", (void *)p_heap_var);
    printf("[STACK]  Address of stack_var:          %p\n", (void *)&stack_var);

    printf("\n=== Segment Distances ===\n");
    printf("RODATA - TEXT:   %" PRIuPTR " bytes\n", rodata_addr - text_addr);
    printf("DATA   - RODATA: %" PRIuPTR " bytes\n", data_addr - rodata_addr);
    printf("BSS    - DATA:   %" PRIuPTR " bytes\n", bss_addr - data_addr);
    printf("HEAP   - BSS:    %" PRIuPTR " bytes\n", heap_addr - bss_addr);
    printf("STACK  - HEAP:   %" PRIuPTR " bytes\n", stack_addr - heap_addr);

    /* giai phong heap (MISRA 22.1) */
    free(p_heap_var);
    p_heap_var = NULL;
}

/**
 * @brief Ham main.
 * @return 0 neu chay thanh cong.
 */
int main(void)
{
    print_memory_map();
    return 0;
}

/*
 * ===== Ket qua kiem tra =====
 *
 * $ size main
 *    text	   data	    bss	    dec	    hex	filename
 *    2929	    644	     12	   3585	    e01	main
 *
 * $ nm main | grep -i global_init
 * 0000000000004010 D global_init
 *
 * $ nm main | grep -i global_uninit
 * 0000000000004018 B global_uninit
 *
 * D = data, B = bss -> dung voi vung da khai bao.
 */
